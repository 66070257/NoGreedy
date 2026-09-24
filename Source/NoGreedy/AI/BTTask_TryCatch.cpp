// No Greedy! game project

#include "BTTask_TryCatch.h"
#include "MyAICharacter.h"
#include "AIController.h"

UBTTask_TryCatch::UBTTask_TryCatch()
{
	NodeName = TEXT("Try Catch (Melee)");
}

EBTNodeResult::Type UBTTask_TryCatch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (AICon == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AMyAICharacter* Me = Cast<AMyAICharacter>(AICon->GetPawn());
	if (Me == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// Failed = ยังติดคูลดาวน์ ไม่ได้เหวี่ยง -> Sequence ล้ม Selector วนกลับมาไล่ต่อ
	return Me->MeleeAttack() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
