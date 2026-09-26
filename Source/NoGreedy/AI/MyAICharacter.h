#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyAICharacter.generated.h"

/**
 * ร่างของตัวไล่ล่า -- แยกคลาสจากตัวผู้เล่นโดยตั้งใจ (แบบ Lab10 AArenaAICharacter)
 *
 * ไม่มีกล้อง ไม่มี input ไม่มี CrystalCount -- ตัวนี้เก็บคริสตอลไม่ได้อยู่แล้ว
 * เพราะ ACrystal::OnBeginOverlap เช็ค IsPlayerControlled() ไว้
 *
 * สมองอยู่ที่ AMyAIController + Behavior Tree -- ไฟล์นี้เป็นแค่ร่างที่เดินตามคำสั่ง
 *
 * หมายเหตุ: skeletal mesh กับ anim blueprint ไม่ได้อยู่ในไฟล์นี้ ไปตั้งใน
 * BP_MyAICharacter เอา -- เหตุผลเดียวกับคอมเมนต์ท้าย constructor ของ
 * ANoGreedyCharacter คือไม่ให้ C++ อ้างถึง content ตรง ๆ
 */
UCLASS()
class NOGREEDY_API AMyAICharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMyAICharacter();

	/** ความเร็วตอนเดินวน (ยังไม่มีเป้า) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Movement")
	float PatrolSpeed = 250.f;

	/** ความเร็วตอนไล่ -- 480 ต้องอยู่ระหว่างผู้เล่นตัวเปล่า 600 กับผู้เล่นถือ 7 ชิ้น 460 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Movement")
	float ChaseSpeed = 480.f;

	/** Behavior Tree เรียกตอนสลับกิ่ง ไล่ <-> เดินวน */
	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	void SetChasing(bool bChasing);

	/**
	 * ระยะตี (จุดกลางถึงจุดกลาง) -- ต้องเท่ากับ SightRadius ใน AMyAIController
	 * MoveTo นับ AcceptRadius จาก "ขอบแคปซูล" ทั้งสองตัว -> ระยะที่หยุดจริง = AcceptRadius + 42 + 42
	 * ตั้ง AcceptRadius ≤ 50 ไม่งั้น AI หยุดก่อนเข้าระยะมองเห็น/ระยะตี
	 */
	UPROPERTY(EditAnywhere, Category = "AI|Melee")
	float MeleeRange = 150.f;

	/** ครึ่งมุมกรวยด้านหน้า (องศา) -- ต้องเท่ากับ PeripheralVisionAngle ของ Sight ใน AMyAIController */
	UPROPERTY(EditAnywhere, Category = "AI|Melee")
	float MeleeHalfAngle = 180.f;

	/** เวลาขั้นต่ำระหว่างการเหวี่ยงแต่ละครั้ง (วินาที) -- ตรงกับคูลดาวน์หลังจับ 1.5 วิในวอลต์ */
	UPROPERTY(EditAnywhere, Category = "AI|Melee")
	float MinAttackInterval = 1.5f;

	/** ท่าตี -- ตั้งใน BP_MyAICharacter (เช่น AM_ComboAttack หรือ montage ที่ทำจาก MM_Attack_01) */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Melee")
	class UAnimMontage* AttackMontage;

	/**
	 * SERVER ONLY -- ถ้ามีผู้เล่นในกรวยหน้า เหวี่ยงหนึ่งทีแล้วโดนหมดทุกคนในกรวย
	 * @return false ถ้ายังติดคูลดาวน์ หรือไม่มีใครในระยะ (ไม่เหวี่ยง ไม่เสียคูลดาวน์)
	 */
	bool MeleeAttack();

	/** เล่นท่าตีทุกเครื่อง -- ของประดับล้วน ไม่มีดาเมจในนี้ */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayAttack();

private:

	float LastAttackTime = -1000.f;
};
