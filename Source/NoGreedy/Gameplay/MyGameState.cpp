// No Greedy! game project

#include "MyGameState.h"
#include "MyPlayerState.h"
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
	Winner = nullptr;

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

FText AMyGameState::GetScoreboardText(const APlayerState* LocalPlayer) const
{
	FString Result;
	for (APlayerState* PS : PlayerArray)
	{
		if (const AMyPlayerState* MPS = Cast<AMyPlayerState>(PS))
		{
			FString Line = FString::Printf(TEXT("%s : %d%s"),
				*MPS->GetPlayerName(), MPS->CrystalCount,
				(MPS == Winner) ? TEXT(" WINNER") : TEXT(""));

			// ตัวเอง -> แท็ก <Me> ตรงกับแถวชื่อ "Me" ใน Text Style Set ของ Rich Text Block
			if (MPS == LocalPlayer)
			{
				Line = FString::Printf(TEXT("<Me>%s</>"), *Line);
			}
			Result += Line + TEXT("\n");
		}
	}
	return FText::FromString(Result);
}

void AMyGameState::RecalculateLeader()
{
	// ใครนำ = เรื่องของ server เท่านั้น client แค่รอค่าที่ replicate มา
	if (!HasAuthority())
	{
		return;
	}

	// เริ่มจากเป้าเดิม -> ถือเท่ากันจะไม่เปลี่ยนเป้า (กฎใน "กฎคนนำโดนล่า")
	AMyPlayerState* Best = CurrentLeader;

	// เป้าเดิมอาจหลุดออกจากเกมหรือเพิ่งโดนจับ -- เช็คก่อนเอาไปเทียบ
	const bool bLeaderGone = !IsValid(Best) || Best->bEliminated;
	if (bLeaderGone)
	{
		Best = nullptr;
	}

	for (APlayerState* PS : PlayerArray)
	{
		AMyPlayerState* Candidate = Cast<AMyPlayerState>(PS);
		// คนตกรอบแล้วไม่นับเป็นคนนำ
		if (Candidate == nullptr || Candidate->bEliminated)
		{
			continue;
		}

		// เปลี่ยนเป้าเมื่อมีคน "มากกว่า" เป้าเดิมเท่านั้น เท่ากันไม่เปลี่ยน
		if (Best == nullptr || Candidate->CrystalCount > Best->CrystalCount)
		{
			Best = Candidate;
		}
	}

	// ทุกคนถือ 0 ชิ้น = ยังไม่มีคนนำ -> AI ยืนนิ่ง
	if (Best != nullptr && Best->CrystalCount <= 0)
	{
		Best = nullptr;
	}

	if (Best == CurrentLeader)
	{
		return;
	}

	// ล็อก 1 วิหลังเปลี่ยนเป้า กันสลับไปมา
	// แต่ถ้าเป้าหายไปเลย (ไม่มีใครนำแล้ว / เป้าโดนจับ) ต้องเปลี่ยนทันที ไม่ต้องรอล็อก
	const float Now = GetWorld()->GetTimeSeconds();
	if (!bLeaderGone && Best != nullptr && Now < LeaderLockUntil)
	{
		return;
	}

	CurrentLeader = Best;
	LeaderLockUntil = Now + LeaderLockDuration;

	// RepNotify ไม่ยิงให้ server ต้องเรียกเอง (เหมือน AMyPlayerState::AddCrystals)
	OnRep_CurrentLeader();
}

void AMyGameState::OnRep_CurrentLeader()
{
	OnLeaderChanged.Broadcast(CurrentLeader);
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

			// ตัดสินบน server เท่านั้น client รอ Winner ที่ replicate มา
			if (HasAuthority())
			{
				DecideWinnerByCrystals();
			}
		}
	}
	else
	{
		// รอบใหม่เริ่มแล้ว เปิดให้ยิง time-up ได้อีกครั้ง
		bHasFiredTimeUp = false;
	}
}

void AMyGameState::SetWinner(AMyPlayerState* NewWinner)
{
	if (!HasAuthority() || Winner != nullptr || NewWinner == nullptr)
	{
		return;
	}

	Winner = NewWinner;
	StopRound();

	UE_LOG(LogTemp, Warning, TEXT("WINNER: %s"), *Winner->GetPlayerName());
}

void AMyGameState::CheckLastSurvivor()
{
	if (!HasAuthority())
	{
		return;
	}

	AMyPlayerState* Survivor = nullptr;
	int32 SurvivorCount = 0;
	for (APlayerState* PS : PlayerArray)
	{
		AMyPlayerState* MPS = Cast<AMyPlayerState>(PS);
		if (MPS && !MPS->bEliminated)
		{
			Survivor = MPS;
			++SurvivorCount;
		}
	}

	if (SurvivorCount == 1)
	{
		SetWinner(Survivor);
	}
}

void AMyGameState::DecideWinnerByCrystals()
{
	AMyPlayerState* Best = nullptr;
	bool bTie = false;
	for (APlayerState* PS : PlayerArray)
	{
		AMyPlayerState* MPS = Cast<AMyPlayerState>(PS);
		if (MPS == nullptr || MPS->bEliminated)
		{
			continue;
		}

		if (Best == nullptr || MPS->CrystalCount > Best->CrystalCount)
		{
			Best = MPS;
			bTie = false;
		}
		else if (MPS->CrystalCount == Best->CrystalCount)
		{
			bTie = true;
		}
	}

	// TODO: เสมอ -> เริ่มรอบใหม่ (GDD ข้อ 8) ตอนนี้แค่ไม่ประกาศผู้ชนะ
	if (Best != nullptr && !bTie)
	{
		SetWinner(Best);
	}
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, RemainingTime);
	DOREPLIFETIME(AMyGameState, bTimerRunning);
	DOREPLIFETIME(AMyGameState, CurrentLeader);
	DOREPLIFETIME(AMyGameState, Winner);
}
