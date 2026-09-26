#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SelectLeader.generated.h"

UCLASS()
class NOGREEDY_API UBTService_SelectLeader : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SelectLeader();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LeaderActorKey;
};
