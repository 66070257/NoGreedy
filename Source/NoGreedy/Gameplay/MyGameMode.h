// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TimerManager.h"
#include "MyGameMode.generated.h"

class AMyGameState;

/**
 * Drives the round countdown on the server: starts it when the match begins,
 * ticks AMyGameState::RemainingTime once a second, and hands off to
 * HandleRoundTimeUp() when it reaches zero (see "นาฬิกา" / "คลาส C++" in the design vault).
 */
UCLASS()
class NOGREEDY_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	AMyGameMode();

	virtual void HandleMatchHasStarted() override;

	/** เริ่มนบเวลารอบใหม่ -- ใช้ทั้งตอนเริ่มเกมและตอนรีเซ็ตรอบเมื่อเสมอ */
	UFUNCTION(BlueprintCallable, Category = "Greed|Timer")
	void StartRound();

protected:

	/** ความถี่ในการอัปเดตนาฬิกา (วินาที) -- 1 วิพอสำหรับ HUD mm:ss */
	UPROPERTY(EditDefaultsOnly, Category = "Greed|Timer")
	float TimerTickInterval = 1.f;

	/** เรียกตอนเวลาหมด -- จุดนี้ยังไม่ตัดสินผู้ชนะ (รอระบบเช็คชนะ) แค่หยุดนาฬิกา */
	UFUNCTION(BlueprintNativeEvent, Category = "Greed|Timer")
	void HandleRoundTimeUp();
	virtual void HandleRoundTimeUp_Implementation();

private:

	void TickRoundTimer();
	AMyGameState* GetMyGameState() const;

	FTimerHandle TimerHandle_RoundTick;
	float RoundStartServerTime = 0.f;
};
