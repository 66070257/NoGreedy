// No Greedy! game project

#include "MyAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "NoGreedyGameMode.h"

AMyAICharacter::AMyAICharacter()
{
	// ไม่ต้อง tick -- Behavior Tree เป็นคนขับ pawn ตัวนี้
	PrimaryActorTick.bCanEverTick = false;

	// แคปซูลเท่าตัวผู้เล่น (42 x 96) ไม่งั้น mesh มาตรฐานจะโผล่พ้นแคปซูล
	// ค่า default ของ wizard คือ 34 x 88 -- คนละขนาดกับ ANoGreedyCharacter
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// AI เลี้ยวด้วยทิศที่เดิน ไม่ใช่ด้วย control rotation (ไม่มีคนถือเมาส์)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// อนิเมชันเดินอ่านค่า acceleration แต่การเดินตาม path ไม่ได้ตั้งค่านี้ให้เอง
	// ถ้าไม่เปิด AI จะไถลไปทั้งตัวโดยขาไม่ขยับ (Lab08 ขั้น 3a)
	GetCharacterMovement()->GetNavMovementProperties()->bUseAccelerationForPaths = true;

	// กันอาการ "วางลงแมปแล้วไม่มีสมอง" -- ค่า default ครอบไม่ถึงตัวที่วางด้วยมือ
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AMyAICharacter::SetChasing(bool bChasing)
{
	GetCharacterMovement()->MaxWalkSpeed = bChasing ? ChaseSpeed : PatrolSpeed;
}

bool AMyAICharacter::MeleeAttack()
{
	// ใครโดนตี = server ตัดสินเท่านั้น
	if (!HasAuthority())
	{
		return false;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < MinAttackInterval)
	{
		return false;
	}
	LastAttackTime = Now;

	// เป้าที่ตีได้มีแค่ผู้เล่น -> ไล่จาก PlayerArray ตรง ๆ ไม่ต้องมี sphere component แบบ Lab
	// (ไม่ต้องไปตั้ง collision ให้ overlap กับแคปซูล ซึ่งพังเงียบได้)
	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS == nullptr)
	{
		return true;
	}

	const FVector Origin = GetActorLocation();
	const FVector Forward = GetActorForwardVector();
	const float CosThresh = FMath::Cos(FMath::DegreesToRadians(MeleeHalfAngle));

	for (APlayerState* PS : GS->PlayerArray)
	{
		APawn* T = PS ? PS->GetPawn() : nullptr;
		if (!IsValid(T))
		{
			continue;
		}

		const FVector To = T->GetActorLocation() - Origin;

		// range gate
		if (To.Size() > MeleeRange)
		{
			continue;
		}

		// cone gate
		if (FVector::DotProduct(Forward, To.GetSafeNormal()) < CosThresh)
		{
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("CATCH: %s hit %s"), *GetName(), *T->GetName());

		// ตีทีเดียวตาย -- ผู้เล่นตีใครไม่ได้ และคนฆ่าได้มีแค่ AI ตัวนี้
		// จึงสั่งผู้คุมกฎตรง ๆ ไม่ต้องผ่าน ApplyDamage -> TakeDamage ของผู้เล่นแบบ Lab
		// (EliminatePlayer กันตายซ้ำด้วย bEliminated อยู่แล้ว)
		if (ANoGreedyGameMode* GM = GetWorld()->GetAuthGameMode<ANoGreedyGameMode>())
		{
			GM->EliminatePlayer(T->GetController(), GetController());
		}
	}

	// เหวี่ยงแล้วโชว์ท่าเสมอ ถึงจะวืดก็ตาม -- หนึ่ง multicast ต่อหนึ่งการเหวี่ยง
	MulticastPlayAttack();
	return true;
}

void AMyAICharacter::MulticastPlayAttack_Implementation()
{
	// Runs on server + all clients. COSMETIC ONLY -- no damage here.
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}
