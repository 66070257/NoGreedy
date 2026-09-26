#include "BTService_MeleeAttack.h"
#include "NoGreedyAICharacter.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
UBTService_MeleeAttack::UBTService_MeleeAttack()
{
	NodeName = TEXT("Melee Attack While Moving");

	Interval = 0.1f;
	RandomDeviation = 0.0f;

	bNotifyTick = true;
	bCallTickOnSearchStart = true;
}

void UBTService_MeleeAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (AICon == nullptr)
	{
		return;
	}

	ANoGreedyAICharacter* Me = Cast<ANoGreedyAICharacter>(AICon->GetPawn());
	if (Me == nullptr)
	{
		return;
	}

	Me->MeleeAttack();
}
