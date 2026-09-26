#include "MyGameState.h"
#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"

AMyGameState::AMyGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	RemainingTime = RoundDuration;
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();

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

	RemainingTime = FMath::Max(RemainingTime - DeltaTime, 0.f);

	if (RemainingTime <= 0.f)
	{
		bTimerRunning = false;
	}

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
	if (!HasAuthority())
	{
		return;
	}

	AMyPlayerState* Best = CurrentLeader;

	const bool bLeaderGone = !IsValid(Best) || Best->bEliminated;
	if (bLeaderGone)
	{
		Best = nullptr;
	}

	for (APlayerState* PS : PlayerArray)
	{
		AMyPlayerState* Candidate = Cast<AMyPlayerState>(PS);
		if (Candidate == nullptr || Candidate->bEliminated)
		{
			continue;
		}

		if (Best == nullptr || Candidate->CrystalCount > Best->CrystalCount)
		{
			Best = Candidate;
		}
	}

	if (Best != nullptr && Best->CrystalCount <= 0)
	{
		Best = nullptr;
	}

	if (Best == CurrentLeader)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (!bLeaderGone && Best != nullptr && Now < LeaderLockUntil)
	{
		return;
	}

	CurrentLeader = Best;
	LeaderLockUntil = Now + LeaderLockDuration;

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

			if (HasAuthority())
			{
				DecideWinnerByCrystals();
			}
		}
	}
	else
	{
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
