// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"
#include "Net/UnrealNetwork.h"

AMyGameState::AMyGameState()
{
}

void AMyGameState::SetRemainingTime(float NewRemainingTime)
{
	// เฉพาะ server เท่านั้นที่เป็นเจ้าของเวลาของรอบได้
	if (!HasAuthority())
	{
		return;
	}

	RemainingTime = FMath::Max(NewRemainingTime, 0.f);

	// RepNotify ไม่ทำงานฝั่ง server เอง ต้องเรียกเองเหมือน AMyPlayerState::AddCrystals
	OnRep_RemainingTime();
}

int32 AMyGameState::GetRemainingSeconds() const
{
	return FMath::CeilToInt(RemainingTime);
}

FString AMyGameState::GetRemainingTimeAsString() const
{
	const int32 TotalSeconds = GetRemainingSeconds();
	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;
	return FString::Printf(TEXT("%d:%02d"), Minutes, Seconds);
}

void AMyGameState::OnRep_RemainingTime()
{
	OnRemainingTimeChanged.Broadcast(RemainingTime);

	if (RemainingTime <= 0.f)
	{
		if (!bHasFiredTimeUp)
		{
			bHasFiredTimeUp = true;
			OnRoundTimeUp.Broadcast();
		}
	}
	else
	{
		// รอบใหม่เริ่มแล้ว (เช่นตอนเสมอแล้วรีเซ็ตรอบ) เปิดให้ยิง time-up ได้อีกครั้ง
		bHasFiredTimeUp = false;
	}
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, RemainingTime);
}
