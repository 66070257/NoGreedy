#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_TryCatch.generated.h"

UCLASS()
class NOGREEDY_API UBTService_TryCatch : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_TryCatch();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
