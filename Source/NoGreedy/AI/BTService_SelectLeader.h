#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SelectLeader.generated.h"

/**
 *  Behavior Tree service that writes the current crystal leader's pawn into the blackboard,
 *  so the AI always chases whoever is holding the most crystals.
 *  The key is cleared when there is no leader or the leader has no pawn.
 */
UCLASS()
class NOGREEDY_API UBTService_SelectLeader : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SelectLeader();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/** Blackboard Object key (Actor) that receives the leader's pawn */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LeaderActorKey;
};
