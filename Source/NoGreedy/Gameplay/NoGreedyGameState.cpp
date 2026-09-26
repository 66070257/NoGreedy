#include "NoGreedyGameState.h"
#include "NoGreedyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "NoGreedy.h"
#include "TimerManager.h"

void ANoGreedyGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && bAutoStartRound)
	{
		StartRound();
	}
}

void ANoGreedyGameState::StartRound()
{
	if (!HasAuthority())
	{
		return;
	}

	RoundEndTime = GetServerWorldTimeSeconds() + RoundDuration;
	bTimerRunning = true;
	bRoundOver = false;
	Winner = nullptr;

	GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &ANoGreedyGameState::OnRoundTimerExpired, RoundDuration);
}

void ANoGreedyGameState::StopRound()
{
	if (!HasAuthority() || !bTimerRunning)
	{
		return;
	}

	FrozenRemainingTime = GetRemainingTime();
	bTimerRunning = false;

	GetWorldTimerManager().ClearTimer(RoundTimerHandle);
}

void ANoGreedyGameState::OnRoundTimerExpired()
{
	FrozenRemainingTime = 0.f;
	bTimerRunning = false;

	OnRoundTimeUp.Broadcast();
	DecideWinnerByCrystals();
}

void ANoGreedyGameState::OnRep_TimerRunning()
{
	if (!bTimerRunning && FrozenRemainingTime == 0.f)
	{
		OnRoundTimeUp.Broadcast();
	}
}

void ANoGreedyGameState::OnRep_RoundOver()
{
	if (bRoundOver)
	{
		OnRoundOver.Broadcast(Winner);
	}
}

float ANoGreedyGameState::GetRemainingTime() const
{
	if (bTimerRunning)
	{
		return FMath::Max(RoundEndTime - GetServerWorldTimeSeconds(), 0.f);
	}

	return FrozenRemainingTime >= 0.f ? FrozenRemainingTime : RoundDuration;
}

int32 ANoGreedyGameState::GetRemainingSeconds() const
{
	return FMath::CeilToInt(GetRemainingTime());
}

FString ANoGreedyGameState::GetRemainingTimeAsString() const
{
	const int32 TotalSeconds = GetRemainingSeconds();
	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;
	return FString::Printf(TEXT("%d:%02d"), Minutes, Seconds);
}

FText ANoGreedyGameState::GetRemainingTimeAsText() const
{
	return FText::FromString(GetRemainingTimeAsString());
}

FText ANoGreedyGameState::GetScoreboardText(const APlayerState* LocalPlayer) const
{
	FString Result;
	if (bRoundOver && Winner == nullptr)
	{
		Result += TEXT("DRAW\n");
	}

	for (APlayerState* PS : PlayerArray)
	{
		if (const ANoGreedyPlayerState* MPS = Cast<ANoGreedyPlayerState>(PS))
		{
			FString Line = FString::Printf(TEXT("%s : %d%s"),
				*MPS->GetPlayerName(), MPS->GetCrystalCount(),
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

void ANoGreedyGameState::RecalculateLeader()
{
	if (!HasAuthority())
	{
		return;
	}

	ANoGreedyPlayerState* Best = CurrentLeader;

	const bool bLeaderGone = !IsValid(Best) || Best->IsEliminated();
	if (bLeaderGone)
	{
		Best = nullptr;
	}

	for (APlayerState* PS : PlayerArray)
	{
		ANoGreedyPlayerState* Candidate = Cast<ANoGreedyPlayerState>(PS);
		if (Candidate == nullptr || Candidate->IsEliminated())
		{
			continue;
		}

		if (Best == nullptr || Candidate->GetCrystalCount() > Best->GetCrystalCount())
		{
			Best = Candidate;
		}
	}

	if (Best != nullptr && Best->GetCrystalCount() <= 0)
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
		// Re-check once the lock expires, otherwise the stale leader stays until the next crystal change
		GetWorldTimerManager().SetTimer(LeaderLockTimerHandle, this, &ANoGreedyGameState::RecalculateLeader, LeaderLockUntil - Now, false);
		return;
	}

	CurrentLeader = Best;
	LeaderLockUntil = Now + LeaderLockDuration;

	OnRep_CurrentLeader();
}

void ANoGreedyGameState::OnRep_CurrentLeader()
{
	OnLeaderChanged.Broadcast(CurrentLeader);
}

void ANoGreedyGameState::SetWinner(ANoGreedyPlayerState* NewWinner)
{
	if (NewWinner == nullptr)
	{
		return;
	}

	EndRound(NewWinner);
}

void ANoGreedyGameState::EndRound(ANoGreedyPlayerState* NewWinner)
{
	if (!HasAuthority() || bRoundOver)
	{
		return;
	}

	StopRound();

	Winner = NewWinner;
	bRoundOver = true;

	OnRep_RoundOver();

	if (Winner)
	{
		UE_LOG(LogNoGreedy, Warning, TEXT("WINNER: %s"), *Winner->GetPlayerName());
	}
	else
	{
		UE_LOG(LogNoGreedy, Warning, TEXT("DRAW"));
	}
}

void ANoGreedyGameState::CheckLastSurvivor()
{
	if (!HasAuthority())
	{
		return;
	}

	ANoGreedyPlayerState* Survivor = nullptr;
	int32 SurvivorCount = 0;
	for (APlayerState* PS : PlayerArray)
	{
		ANoGreedyPlayerState* MPS = Cast<ANoGreedyPlayerState>(PS);
		if (MPS && !MPS->IsEliminated())
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

void ANoGreedyGameState::DecideWinnerByCrystals()
{
	ANoGreedyPlayerState* Best = nullptr;
	bool bTie = false;
	for (APlayerState* PS : PlayerArray)
	{
		ANoGreedyPlayerState* MPS = Cast<ANoGreedyPlayerState>(PS);
		if (MPS == nullptr || MPS->IsEliminated())
		{
			continue;
		}

		if (Best == nullptr || MPS->GetCrystalCount() > Best->GetCrystalCount())
		{
			Best = MPS;
			bTie = false;
		}
		else if (MPS->GetCrystalCount() == Best->GetCrystalCount())
		{
			bTie = true;
		}
	}

	EndRound(bTie ? nullptr : Best);
}

void ANoGreedyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANoGreedyGameState, bTimerRunning);
	DOREPLIFETIME(ANoGreedyGameState, RoundEndTime);
	DOREPLIFETIME(ANoGreedyGameState, FrozenRemainingTime);
	DOREPLIFETIME(ANoGreedyGameState, CurrentLeader);
	DOREPLIFETIME(ANoGreedyGameState, Winner);
	DOREPLIFETIME(ANoGreedyGameState, bRoundOver);
}
