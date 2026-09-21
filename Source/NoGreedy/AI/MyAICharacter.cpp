// No Greedy! game project

#include "MyAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
