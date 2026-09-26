#include "BTService_TryCatch.h"
#include "MyAICharacter.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
UBTService_TryCatch::UBTService_TryCatch()
{
	NodeName = TEXT("Try Catch While Moving");

	Interval = 0.1f;
	RandomDeviation = 0.0f;

	bNotifyTick = true;
	bCallTickOnSearchStart = true;
}

void UBTService_TryCatch::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (AICon == nullptr)
	{
		return;
	}

	AMyAICharacter* Me = Cast<AMyAICharacter>(AICon->GetPawn());
	if (Me == nullptr)
	{
		return;
	}

	Me->MeleeAttack();
}
