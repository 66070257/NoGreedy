#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "NoGreedyAIController.generated.h"

class ANoGreedyPlayerState;

/**
 *  Controller for ANoGreedyAICharacter. Runs BehaviorTreeAsset on possess and
 *  shuts the AI down when the GameState reports the round is over.
 */
UCLASS()
class NOGREEDY_API ANoGreedyAIController : public AAIController
{
	GENERATED_BODY()

public:
	/** Starts BehaviorTreeAsset on the possessed pawn */
	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

	/** Stops the behavior tree and movement so the AI stands still once the round is decided */
	UFUNCTION()
	void HandleRoundOver(ANoGreedyPlayerState* RoundWinner);

	/** Tree to run; expected to use UBTService_SelectLeader and UBTService_MeleeAttack */
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
};
