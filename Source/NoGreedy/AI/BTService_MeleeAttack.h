#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_MeleeAttack.generated.h"

/**
 *  Behavior Tree service that keeps trying a melee attack while the AI runs its other tasks (e.g. chasing).
 *  All range, cooldown and round checks live in ANoGreedyAICharacter::MeleeAttack.
 */
UCLASS()
class NOGREEDY_API UBTService_MeleeAttack : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_MeleeAttack();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
