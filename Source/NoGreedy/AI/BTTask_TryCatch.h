#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TryCatch.generated.h"

/**
 * "จับ" = ตีหนึ่งที (แบบ Lab UBTTask_MeleeAttack)
 *
 * วางต่อท้าย MoveTo(LeaderActor) ในกิ่งไล่ -- ตัว task แค่สั่งให้ร่างเหวี่ยง
 * ส่วนเช็คระยะ/มุม/ความเสียหายอยู่ใน AMyAICharacter::MeleeAttack() ทั้งหมด
 *
 * BT รันบน server เท่านั้น (AIController ไม่มีบน client) -> ไม่ต้องใช้ Server RPC
 */
UCLASS()
class NOGREEDY_API UBTTask_TryCatch : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTTask_TryCatch();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
