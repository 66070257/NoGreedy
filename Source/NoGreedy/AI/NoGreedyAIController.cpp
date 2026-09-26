#include "NoGreedyAIController.h"
#include "BrainComponent.h"
#include "Gameplay/NoGreedyGameState.h"

void ANoGreedyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ANoGreedyGameState* GS = GetWorld()->GetGameState<ANoGreedyGameState>())
	{
		GS->OnRoundOver.AddDynamic(this, &ANoGreedyAIController::HandleRoundOver);
	}
}

void ANoGreedyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void ANoGreedyAIController::HandleRoundOver(ANoGreedyPlayerState* RoundWinner)
{
	StopMovement();

	if (BrainComponent)
	{
		BrainComponent->StopLogic(TEXT("Round over"));
	}
}
