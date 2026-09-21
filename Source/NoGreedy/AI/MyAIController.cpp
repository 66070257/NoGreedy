// No Greedy! game project

#include "MyAIController.h"

void AMyAIController::OnPossess(APawn* InPawn)
{
	// ให้ engine ทำงาน possess ของตัวเองให้เสร็จก่อน
	Super::OnPossess(InPawn);

	// บรรทัดนี้อยู่บน SERVER เสมอ -- AIController ไม่มีอยู่บน client
	if (BehaviorTreeAsset)
	{
		// RunBehaviorTree สร้าง Blackboard ให้เองจาก asset ที่ tree ชี้อยู่
		// (ทรีที่ไม่ได้ตั้ง BlackboardAsset = AI ไม่มีความจำ และไม่มี error)
		RunBehaviorTree(BehaviorTreeAsset);
	}
}
