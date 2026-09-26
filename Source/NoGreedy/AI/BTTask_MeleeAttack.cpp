#include "BTTask_MeleeAttack.h"
#include "NoGreedyAICharacter.h"
#include "AIController.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = TEXT("Melee Attack");
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (AICon == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ANoGreedyAICharacter* Me = Cast<ANoGreedyAICharacter>(AICon->GetPawn());
	if (Me == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	return Me->MeleeAttack() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
