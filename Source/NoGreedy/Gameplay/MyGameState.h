// No Greedy! game project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyPlayerState.h"
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

	/**
	 * คริสตอลของผู้เล่นทุกคน บรรทัดละคน -- ต่อเข้า Rich Text Block ของ HUD (แทน timer RefreshScoreboard)
	 * บรรทัดของ LocalPlayer ถูกครอบด้วย <Me>...</> ให้ Rich Text ระบายสีเขียว
	 */
	UFUNCTION(BlueprintPure, Category = "Score")
	FText GetScoreboardText(const APlayerState* LocalPlayer) const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingTimeChanged, float, NewRemainingTime);
	/** ยิงทุกครั้งที่เวลาเปลี่ยน (ทั้ง server และ client) -- widget เอาไป Bind Event ได้ */
	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FOnRemainingTimeChanged OnRemainingTimeChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundTimeUp);
	/** ยิงครั้งเดียวตอนเวลาแตะ 0 */
	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FOnRoundTimeUp OnRoundTimeUp;

	//~ คนนำ -- กฎ "คนนำโดนล่า" ในวอลต์

	/**
	 * ผู้เล่นที่ถือคริสตอลมากที่สุดตอนนี้ -- AI ไล่คนนี้
	 * null = ยังไม่มีใครนำ (ทุกคนถือ 0 ชิ้น) -> AI ยืนนิ่ง
	 */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentLeader, VisibleInstanceOnly, BlueprintReadOnly, Category = "Leader")
	AMyPlayerState* CurrentLeader = nullptr;

	//~ ผู้ชนะ -- GDD: รอดคนเดียวชนะทันที · หมดเวลา = ถือมากสุดชนะ · เสมอ = ไม่มีผู้ชนะ

	/** ผู้ชนะของรอบ null = ยังไม่จบ (หรือเสมอ) -- server ตั้ง, replicate ให้ HUD อ่าน */
	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Winner")
	AMyPlayerState* Winner = nullptr;

	/** Server-only: ประกาศผู้ชนะแล้วหยุดนาฬิกา (ตั้งได้ครั้งเดียวต่อรอบ) */
	void SetWinner(AMyPlayerState* NewWinner);

	/** Server-only: เหลือผู้เล่นรอดคนเดียว -> คนนั้นชนะ */
	void CheckLastSurvivor();

	/** ล็อกกี่วินาทีหลังเปลี่ยนเป้า -- กัน AI สลับไปมาตอนสองคนเก็บ-ทิ้งสลับกัน */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Leader")
	float LeaderLockDuration = 1.0f;

	/** Server-only: เลือกคนนำใหม่ เรียกทุกครั้งที่ CrystalCount ของใครเปลี่ยน */
	UFUNCTION(BlueprintCallable, Category = "Leader")
	void RecalculateLeader();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeaderChanged, AMyPlayerState*, NewLeader);
	/** ยิงทุกครั้งที่คนนำเปลี่ยน (ทั้ง server และ client) -- เอาไปทำไอคอนมงกุฎเหนือหัวได้ */
	UPROPERTY(BlueprintAssignable, Category = "Leader")
	FOnLeaderChanged OnLeaderChanged;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_CurrentLeader();

	UFUNCTION()
	void OnRep_RemainingTime();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	/** ยิง event เวลาเปลี่ยน + เช็คหมดเวลา ใช้ร่วมกันทั้งฝั่ง server (Tick) และ client (OnRep) */
	void HandleRemainingTimeChanged();

	/** Server-only: หมดเวลา -> คนรอดที่ถือมากสุดชนะ เท่ากัน = ไม่ประกาศ */
	void DecideWinnerByCrystals();

	/** กันยิง OnRoundTimeUp ซ้ำ */
	bool bHasFiredTimeUp = false;

	/** เวลาที่เปลี่ยนเป้าได้อีกครั้ง (วินาทีของโลก) -- server เท่านั้นที่ใช้ */
	float LeaderLockUntil = 0.f;
};
