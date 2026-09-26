#include "MyAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "NoGreedyGameMode.h"

AMyAICharacter::AMyAICharacter()
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

void AMyAICharacter::SetChasing(bool bChasing)
{
	GetCharacterMovement()->MaxWalkSpeed = bChasing ? ChaseSpeed : PatrolSpeed;
}

bool AMyAICharacter::MeleeAttack()
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

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS == nullptr)
	{
		return false;
	}

	const FVector Origin = GetActorLocation();
	const FVector Forward = GetActorForwardVector();
	const float CosThresh = FMath::Cos(FMath::DegreesToRadians(MeleeHalfAngle));

	TArray<APawn*> Hits;
	for (APlayerState* PS : GS->PlayerArray)
	{
		APawn* T = PS ? PS->GetPawn() : nullptr;
		if (!IsValid(T))
		{
			continue;
		}

		const FVector To = T->GetActorLocation() - Origin;

		if (To.Size() > MeleeRange)
		{
			continue;
		}

		if (FVector::DotProduct(Forward, To.GetSafeNormal()) < CosThresh)
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
		UE_LOG(LogTemp, Warning, TEXT("CATCH: %s hit %s"), *GetName(), *T->GetName());

		if (ANoGreedyGameMode* GM = GetWorld()->GetAuthGameMode<ANoGreedyGameMode>())
		{
			GM->EliminatePlayer(T->GetController(), GetController());
		}
	}

	MulticastPlayAttack();
	return true;
}

void AMyAICharacter::MulticastPlayAttack_Implementation()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}
