#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTSelectLeader.generated.h"

/**
 * อ่าน "ใครคือคนนำ" จาก GameState แล้วเขียน pawn ของคนนั้นลง Blackboard
 *
 * นี่คือจุดที่ Wk7 (GameState) กับ Wk8 (Behavior Tree) ต่อกันตรง ๆ ผ่าน custom node (Wk11)
 * -- AI ไม่ได้ "เดาเอง" ว่าใครนำ แต่ไปอ่านค่าที่ server ตัดสินไว้แล้ว
 *
 * ไม่มีคนนำ (ทุกคนถือ 0 / คนนำเพิ่งตกรอบ) -> เคลียร์คีย์ -> ทรีตกไปกิ่ง Idle = AI ยืนนิ่ง
 */
UCLASS()
class NOGREEDY_API UBTSelectLeader : public UBTService
{
	GENERATED_BODY()

public:

	UBTSelectLeader();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/** คีย์ปลายทาง -- ตั้งเป็น LeaderActor ในทรี */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LeaderActorKey;
};
