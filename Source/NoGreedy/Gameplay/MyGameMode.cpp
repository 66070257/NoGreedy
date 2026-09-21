// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "MyGameState.h"

AMyGameMode::AMyGameMode()
{
}

void AMyGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	StartRound();
}

void AMyGameMode::StartRound()
{
	if (!HasAuthority())
	{
		return;
	}

	AMyGameState* MyGameState = GetMyGameState();
	if (!MyGameState)
	{
		return;
	}

	RoundStartServerTime = GetWorld()->GetTimeSeconds();
	MyGameState->SetRemainingTime(MyGameState->RoundDuration);

	GetWorldTimerManager().ClearTimer(TimerHandle_RoundTick);
	GetWorldTimerManager().SetTimer(TimerHandle_RoundTick, this, &AMyGameMode::TickRoundTimer, TimerTickInterval, true);
}

void AMyGameMode::TickRoundTimer()
{
	AMyGameState* MyGameState = GetMyGameState();
	if (!MyGameState)
	{
		return;
	}

	// คำนวณจากเวลาเซิร์ฟเวอร์จริง ไม่ใช้การลดทีละ tick เพื่อกัน drift สะสม
	const float Elapsed = GetWorld()->GetTimeSeconds() - RoundStartServerTime;
	const float Remaining = MyGameState->RoundDuration - Elapsed;

	if (Remaining <= 0.f)
	{
		MyGameState->SetRemainingTime(0.f);
		GetWorldTimerManager().ClearTimer(TimerHandle_RoundTick);
		HandleRoundTimeUp();
		return;
	}

	MyGameState->SetRemainingTime(Remaining);
}

void AMyGameMode::HandleRoundTimeUp_Implementation()
{
	// TODO: ต่อกับระบบเช็คชนะ (ถือคริสตอลมากสุดตอนหมดเวลา) เมื่อ AMyGameMode::เช็คชนะ พร้อม
}

AMyGameState* AMyGameMode::GetMyGameState() const
{
	return GetGameState<AMyGameState>();
}
