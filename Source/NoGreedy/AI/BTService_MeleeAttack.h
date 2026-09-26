#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_MeleeAttack.generated.h"

UCLASS()
class NOGREEDY_API UBTService_MeleeAttack : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_MeleeAttack();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
