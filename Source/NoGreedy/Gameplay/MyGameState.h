// No Greedy! game project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameState.generated.h"

/**
 * นาฬิกานับถอยหลังของรอบ (300 วินาที ตามเอกสารเสนอ)
 *
 * นับเวลาด้วย DeltaTime ใน Tick() แบบเดียวกับ Week 2 (AMyCppActor::Tick)
 * -- ไม่ใช้ TimerManager / SetTimer อีกต่อไป
 *
 * server เป็นเจ้าของเวลาจริงแล้ว replicate ค่ามาให้ทุกเครื่อง
 * ส่วน client ก็นับลงเองทุกเฟรมด้วย เพื่อให้ HUD เดินลื่น ไม่กระตุกตามรอบ network
 */
UCLASS()
class NOGREEDY_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	AMyGameState();

	/** นับถอยหลังทุกเฟรม -- หัวใจของระบบจับเวลาใหม่ */
	virtual void Tick(float DeltaTime) override;

	/** ความยาวของหนึ่งรอบ (วินาที) -- 300 วิ มาจากเอกสารเสนอ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	float RoundDuration = 300.f;

	/** เริ่มนับอัตโนมัติตอนเข้าเกมเลยไหม ถ้าปิดต้องเรียก StartRound() เอง */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	bool bAutoStartRound = true;

	/** เวลาที่เหลือของรอบปัจจุบัน (วินาที) */
	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, VisibleInstanceOnly, BlueprintReadOnly, Category = "Timer")
	float RemainingTime = 300.f;

	/** นาฬิกากำลังเดินอยู่หรือเปล่า -- หยุดเดินเมื่อเวลาหมดหรือเรียก StopRound() */
	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Timer")
	bool bTimerRunning = false;

	/** Server-only: เริ่มรอบใหม่ ตั้งเวลากลับไปที่ RoundDuration แล้วเริ่มเดิน */
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StartRound();

	/** Server-only: หยุดนาฬิกาค้างไว้ที่เวลาปัจจุบัน */
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StopRound();

	/** เวลาที่เหลือ ปัดขึ้นเป็นวินาทีเต็ม */
	UFUNCTION(BlueprintPure, Category = "Timer")
	int32 GetRemainingSeconds() const;

	/** เวลาที่เหลือในรูปแบบ mm:ss เช่น "5:00" */
	UFUNCTION(BlueprintPure, Category = "Timer")
	FString GetRemainingTimeAsString() const;

	/** เหมือน GetRemainingTimeAsString แต่เป็น Text -- ต่อเข้า Text Block ของ HUD ได้ตรง ๆ */
	UFUNCTION(BlueprintPure, Category = "Timer")
	FText GetRemainingTimeAsText() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingTimeChanged, float, NewRemainingTime);
	/** ยิงทุกครั้งที่เวลาเปลี่ยน (ทั้ง server และ client) -- widget เอาไป Bind Event ได้ */
	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FOnRemainingTimeChanged OnRemainingTimeChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundTimeUp);
	/** ยิงครั้งเดียวตอนเวลาแตะ 0 */
	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FOnRoundTimeUp OnRoundTimeUp;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_RemainingTime();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	/** ยิง event เวลาเปลี่ยน + เช็คหมดเวลา ใช้ร่วมกันทั้งฝั่ง server (Tick) และ client (OnRep) */
	void HandleRemainingTimeChanged();

	/** กันยิง OnRoundTimeUp ซ้ำ */
	bool bHasFiredTimeUp = false;
};
