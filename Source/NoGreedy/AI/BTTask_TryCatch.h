#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TryCatch.generated.h"

UCLASS()
class NOGREEDY_API UBTTask_TryCatch : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TryCatch();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
