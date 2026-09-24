// No Greedy! game project

#include "MyAIController.h"
#include "MyAICharacter.h"
#include "Gameplay/MyGameState.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "TimerManager.h"

AMyAIController::AMyAIController()
{
	// This component must exist before any sense
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 150.f;						// = MeleeRange
	SightConfig->LoseSightRadius = 200.f;					// เผื่อไว้กันเห็น-หลุดสลับรัว ๆ ตรงขอบ
	SightConfig->PeripheralVisionAngleDegrees = 180.f;		// = MeleeHalfAngle

	// เกมนี้ไม่มีทีม -> ผู้เล่นทุกคนเป็น Neutral ถ้าเปิดแค่ Enemies แบบ Lab จะมองไม่เห็นใครเลย
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	GetPerceptionComponent()->ConfigureSense(*SightConfig);

	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AMyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (UAIPerceptionComponent* Perception = GetPerceptionComponent())
	{
		Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AMyAIController::OnTargetSensed);
	}
}

void AMyAIController::OnPossess(APawn* InPawn)
{
	// ให้ engine ทำงาน possess ของตัวเองให้เสร็จก่อน
	Super::OnPossess(InPawn);

	// บรรทัดนี้อยู่บน SERVER เสมอ -- AIController ไม่มีอยู่บน client
	if (BehaviorTreeAsset)
	{
		// RunBehaviorTree สร้าง Blackboard ให้เองจาก asset ที่ tree ชี้อยู่
		// (ทรีที่ไม่ได้ตั้ง BlackboardAsset = AI ไม่มีความจำ และไม่มี error)
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AMyAIController::OnTargetSensed(AActor* Actor, FAIStimulus Stimulus)
{
	// หลุดสายตาไม่ต้องทำอะไร -- ใครยังอยู่ในสายตา CatchPerceived ไล่เช็คเอง
	if (!Stimulus.WasSuccessfullySensed())
	{
		return;
	}

	const APawn* SeenPawn = Cast<APawn>(Actor);
	if (SeenPawn == nullptr || !SeenPawn->IsPlayerControlled())
	{
		return;
	}

	CatchPerceived();
}

void AMyAIController::CatchPerceived()
{
	TArray<AActor*> Seen;
	GetPerceptionComponent()->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), Seen);

	bool bPlayerInSight = false;
	for (AActor* A : Seen)
	{
		const APawn* P = Cast<APawn>(A);
		if (IsValid(P) && P->IsPlayerControlled())
		{
			bPlayerInSight = true;
			break;
		}
	}

	// ไม่มีใครในสายตาแล้ว -> หยุดลองซ้ำ รอ Sight ยิง event รอบหน้า
	if (!bPlayerInSight)
	{
		return;
	}

	// ไม่มีคนนำ = AI หลับอยู่ เดินผ่านหน้าไม่โดน (แต่ยังลองซ้ำไว้ เผื่อมีคนนำระหว่างที่ยืนอยู่ตรงนั้น)
	const AMyGameState* GS = GetWorld()->GetGameState<AMyGameState>();
	const bool bHunting = GS && IsValid(GS->CurrentLeader);

	AMyAICharacter* Me = Cast<AMyAICharacter>(GetPawn());
	if (bHunting && Me)
	{
		Me->MeleeAttack();
	}

	// ยังมีคนในสายตา -> ลองใหม่เรื่อย ๆ จนกว่าจะพ้นสายตา
	// ครอบทุกกรณีที่ Sight ไม่ยิง event ซ้ำ: ติดคูลดาวน์ / ยังไม่มีคนนำ / จับคนหนึ่งแล้วอีกคนยังยืนอยู่
	GetWorldTimerManager().SetTimer(CatchRetryHandle, this, &AMyAIController::CatchPerceived, CatchRetryInterval, false);
}
