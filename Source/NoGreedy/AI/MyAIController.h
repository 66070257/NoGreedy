#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionTypes.h"
#include "MyAIController.generated.h"

class UAISenseConfig_Sight;

/**
 * สมองของตัวไล่ล่า
 *
 * AIController มีอยู่บน server เครื่องเดียวเท่านั้น -> ทุกการตัดสินใจของ AI
 * เป็น server-authoritative ตั้งแต่บรรทัดแรก ไม่ต้องเขียน RPC เพิ่มสักตัว
 * client เห็นแค่ pawn ขยับ ผ่าน movement replication ปกติ
 *
 * การจับใช้ Sight (แบบ Lab Wk9): ระยะมองเห็น = ระยะตี -> เห็นปุ๊บจับปั๊บ
 * ส่วน "ใครคือเป้า / ไปทางไหน" ยังมาจาก GameState ผ่าน BTSelectLeader เหมือนเดิม
 */
UCLASS()
class NOGREEDY_API AMyAIController : public AAIController
{
	GENERATED_BODY()

public:

	AMyAIController();

	virtual void OnPossess(APawn* InPawn) override;

protected:

	virtual void BeginPlay() override;

	/** ตั้งใน blueprint ลูก BP_MyAIController -> BT_ArenaAI */
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	/**
	 * ตาสำหรับจับ -- SightRadius ต้องเท่ากับ MeleeRange ของ AMyAICharacter (150)
	 * และ PeripheralVisionAngle เท่ากับ MeleeHalfAngle (60) ไม่งั้นเห็นแล้วตีไม่โดน
	 */
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void OnTargetSensed(AActor* Actor, FAIStimulus Stimulus);

	/**
	 * จับทุกคนที่ยังอยู่ในสายตา · ตราบใดที่ยังมีผู้เล่นในสายตา นัดตัวเองใหม่ทุก 0.2 วิ
	 * จำเป็นเพราะ Sight ยิง event แค่ตอน "เพิ่งเห็น" -- ยืนอยู่ในระยะต่อจะไม่มี event ซ้ำ
	 */
	void CatchPerceived();

	/** เวลาที่รอก่อนลองจับใหม่ */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float CatchRetryInterval = 0.2f;

private:

	FTimerHandle CatchRetryHandle;
};
