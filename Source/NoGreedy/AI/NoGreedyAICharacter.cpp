#include "NoGreedyAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/NoGreedyGameState.h"
#include "NoGreedyGameMode.h"
#include "NoGreedy.h"

ANoGreedyAICharacter::ANoGreedyAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	GetCharacterMovement()->GetNavMovementProperties()->bUseAccelerationForPaths = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ANoGreedyAICharacter::SetChasing(bool bChasing)
{
	GetCharacterMovement()->MaxWalkSpeed = bChasing ? ChaseSpeed : PatrolSpeed;
}

bool ANoGreedyAICharacter::MeleeAttack()
{
	if (!HasAuthority())
	{
		return false;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < MinAttackInterval)
	{
		return false;
	}

	ANoGreedyGameState* GS = GetWorld()->GetGameState<ANoGreedyGameState>();
	ANoGreedyGameMode* GM = GetWorld()->GetAuthGameMode<ANoGreedyGameMode>();
	if (GS == nullptr || GM == nullptr)
	{
		return false;
	}

	// AI only hunts during the round while someone holds the lead, regardless of where the BT calls this from
	if (GS->IsRoundOver() || !IsValid(GS->GetCurrentLeader()))
	{
		return false;
	}

	const FVector Origin = GetActorLocation();

	TArray<APawn*> Hits;
	for (APlayerState* PS : GS->PlayerArray)
	{
		APawn* T = PS ? PS->GetPawn() : nullptr;
		if (!IsValid(T))
		{
			continue;
		}

		if (FVector::Dist(T->GetActorLocation(), Origin) > MeleeRange)
		{
			continue;
		}

		Hits.Add(T);
	}

	if (Hits.Num() == 0)
	{
		return false;
	}

	LastAttackTime = Now;

	APawn* Nearest = Hits[0];
	for (APawn* T : Hits)
	{
		if (FVector::DistSquared(T->GetActorLocation(), Origin) < FVector::DistSquared(Nearest->GetActorLocation(), Origin))
		{
			Nearest = T;
		}
	}
	const FVector ToNearest = Nearest->GetActorLocation() - Origin;
	SetActorRotation(FRotator(0.f, ToNearest.Rotation().Yaw, 0.f));

	for (APawn* T : Hits)
	{
		UE_LOG(LogNoGreedy, Warning, TEXT("CATCH: %s hit %s"), *GetName(), *T->GetName());
		GM->EliminatePlayer(T->GetController(), GetController());
	}

	MulticastPlayAttack();
	return true;
}

void ANoGreedyAICharacter::MulticastPlayAttack_Implementation()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}
