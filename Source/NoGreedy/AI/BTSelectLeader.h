#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTSelectLeader.generated.h"

UCLASS()
class NOGREEDY_API UBTSelectLeader : public UBTService
{
	GENERATED_BODY()

public:
	UBTSelectLeader();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LeaderActorKey;
};
