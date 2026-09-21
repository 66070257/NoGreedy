// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

/**
 * Tracks the round countdown -- the single source of truth for "RemainingTime"
 * that AMyGameMode drives on the server and every HUD reads on the client
 * (see "นาฬิกา" / "คลาส C++" in the design vault).
 */
UCLASS()
class NOGREEDY_API AMyGameState : public AGameState
{
	GENERATED_BODY()

public:

	AMyGameState();

	/** ระยะเวลาต่อรอบ (วินาที) -- ค่าเริ่มต้น 300 วิ มาจากเอกสารเสนอ (ค่าล็อกในเอกสาร) ห้ามแก้โดยไม่ตกลงกันก่อน */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Greed|Timer")
	float RoundDuration = 300.f;

	/** เวลาที่เหลือของรอบปัจจุบัน (วินาที). Authoritative on the server, replicated to every client. */
	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, VisibleAnywhere, BlueprintReadOnly, Category = "Greed|Timer")
	float RemainingTime = 0.f;

	/** Server-only: เซ็ตเวลาที่เหลือใหม่ แล้ว sync ไปทุกเครื่อง (เรียกจาก AMyGameMode เท่านั้น) */
	void SetRemainingTime(float NewRemainingTime);

	/** เวลาที่เหลือ ปัดขึ้นเป็นวินาทีเต็ม สำหรับโชว์ HUD */
	UFUNCTION(BlueprintPure, Category = "Greed|Timer")
	int32 GetRemainingSeconds() const;

	/** แปลงเวลาที่เหลือเป็นรูปแบบ mm:ss เช่น 300 -> "5:00", พร้อมใช้กับ HUD ได้ทันที */
	UFUNCTION(BlueprintPure, Category = "Greed|Timer")
	FString GetRemainingTimeAsString() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingTimeChanged, float, NewRemainingTime);
	/** ยิงทุกครั้งที่ RemainingTime เปลี่ยน (ทั้ง server และ client) -- widget เอาไป "Bind Event to OnRemainingTimeChanged" ได้เลย ไม่ต้อง Tick/poll */
	UPROPERTY(BlueprintAssignable, Category = "Greed|Timer")
	FOnRemainingTimeChanged OnRemainingTimeChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundTimeUp);
	/** ยิงครั้งเดียวตอนเวลาหมด (RemainingTime แตะ 0) ทั้ง server และ client */
	UPROPERTY(BlueprintAssignable, Category = "Greed|Timer")
	FOnRoundTimeUp OnRoundTimeUp;

protected:

	UFUNCTION()
	void OnRep_RemainingTime();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	/** กันยิง OnRoundTimeUp ซ้ำหลายครั้งตอนเวลาแตะ 0 */
	bool bHasFiredTimeUp = false;
};
