#include "MyAIController.h"
#include "MyAICharacter.h"
#include "Gameplay/MyGameState.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "TimerManager.h"

AMyAIController::AMyAIController()
{
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 150.f;
	SightConfig->LoseSightRadius = 200.f;
	SightConfig->PeripheralVisionAngleDegrees = 180.f;

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
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AMyAIController::OnTargetSensed(AActor* Actor, FAIStimulus Stimulus)
{
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

	if (!bPlayerInSight)
	{
		return;
	}

	const AMyGameState* GS = GetWorld()->GetGameState<AMyGameState>();
	const bool bHunting = GS && IsValid(GS->CurrentLeader);

	AMyAICharacter* Me = Cast<AMyAICharacter>(GetPawn());
	if (bHunting && Me)
	{
		Me->MeleeAttack();
	}

	GetWorldTimerManager().SetTimer(CatchRetryHandle, this, &AMyAIController::CatchPerceived, CatchRetryInterval, false);
}
