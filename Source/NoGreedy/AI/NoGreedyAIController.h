#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "NoGreedyAIController.generated.h"

class ANoGreedyPlayerState;

UCLASS()
class NOGREEDY_API ANoGreedyAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

	/** Stops the behavior tree and movement so the AI stands still once the round is decided */
	UFUNCTION()
	void HandleRoundOver(ANoGreedyPlayerState* RoundWinner);

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
};
