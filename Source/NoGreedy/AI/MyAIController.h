// No Greedy! game project

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MyAIController.generated.h"

/**
 * สมองของตัวไล่ล่า
 *
 * AIController มีอยู่บน server เครื่องเดียวเท่านั้น -> ทุกการตัดสินใจของ AI
 * เป็น server-authoritative ตั้งแต่บรรทัดแรก ไม่ต้องเขียน RPC เพิ่มสักตัว
 * client เห็นแค่ pawn ขยับ ผ่าน movement replication ปกติ
 */
UCLASS()
class NOGREEDY_API AMyAIController : public AAIController
{
	GENERATED_BODY()

public:

	virtual void OnPossess(APawn* InPawn) override;

protected:

	/** ตั้งใน blueprint ลูก BP_MyAIController -> BT_ArenaAI */
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;
};
