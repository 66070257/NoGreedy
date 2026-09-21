// No Greedy! game project

#include "MyGameState.h"
#include "Net/UnrealNetwork.h"

AMyGameState::AMyGameState()
{
	// Week 2: เปิด Tick เพื่อนับเวลาเองทุกเฟรมด้วย DeltaTime
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	RemainingTime = RoundDuration;
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();

	// เฉพาะ server เท่านั้นที่เริ่มรอบได้ -- client รอค่าที่ replicate มา
	if (HasAuthority() && bAutoStartRound)
	{
		StartRound();
	}
}

void AMyGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bTimerRunning)
	{
		return;
	}

	// หัวใจของ Week 2: ลดเวลาด้วย DeltaTime ทุกเฟรม
	// ได้ 1 วินาทีจริงเท่ากันทุกเครื่อง ไม่ว่า frame rate จะเท่าไหร่
	RemainingTime = FMath::Max(RemainingTime - DeltaTime, 0.f);

	if (RemainingTime <= 0.f)
	{
		bTimerRunning = false;
	}

	// ฝั่ง server: RepNotify ไม่ยิงให้ตัวเอง ต้องเรียกเอง (เหมือน AMyPlayerState::AddCrystals)
	// ฝั่ง client: นับลงเองให้ HUD ลื่น เดี๋ยวค่าจริงจาก server จะ replicate มาแก้ให้ตรงเอง
	HandleRemainingTimeChanged();
}

void AMyGameState::StartRound()
{
	if (!HasAuthority())
	{
		return;
	}

	RemainingTime = RoundDuration;
	bTimerRunning = true;
	bHasFiredTimeUp = false;

	HandleRemainingTimeChanged();
}

void AMyGameState::StopRound()
{
	if (!HasAuthority())
	{
		return;
	}

	bTimerRunning = false;
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

FText AMyGameState::GetRemainingTimeAsText() const
{
	return FText::FromString(GetRemainingTimeAsString());
}

void AMyGameState::OnRep_RemainingTime()
{
	HandleRemainingTimeChanged();
}

void AMyGameState::HandleRemainingTimeChanged()
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
		// รอบใหม่เริ่มแล้ว เปิดให้ยิง time-up ได้อีกครั้ง
		bHasFiredTimeUp = false;
	}
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, RemainingTime);
	DOREPLIFETIME(AMyGameState, bTimerRunning);
}
