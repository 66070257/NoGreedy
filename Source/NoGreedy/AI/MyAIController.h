#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionTypes.h"
#include "MyAIController.generated.h"

class UAISenseConfig_Sight;

UCLASS()
class NOGREEDY_API AMyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMyAIController();

	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void OnTargetSensed(AActor* Actor, FAIStimulus Stimulus);

	void CatchPerceived();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float CatchRetryInterval = 0.2f;

private:
	FTimerHandle CatchRetryHandle;
};
