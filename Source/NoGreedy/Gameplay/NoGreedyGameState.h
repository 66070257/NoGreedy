#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NoGreedyPlayerState.h"
#include "NoGreedyGameState.generated.h"

UCLASS()
class NOGREEDY_API ANoGreedyGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	bool bTimerRunning = false;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Leader", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ANoGreedyPlayerState> CurrentLeader = nullptr;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Winner", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ANoGreedyPlayerState> Winner = nullptr;

	/** True once the round has ended, with a winner or as a draw (Winner == nullptr) */
	UPROPERTY(ReplicatedUsing = OnRep_RoundOver, VisibleInstanceOnly, BlueprintReadOnly, Category = "Winner", meta = (AllowPrivateAccess = "true"))
	bool bRoundOver = false;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	float RoundDuration = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	bool bAutoStartRound = true;

	ANoGreedyPlayerState* GetCurrentLeader() const { return CurrentLeader; }

	bool IsRoundOver() const { return bRoundOver; }

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StartRound();

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StopRound();

	/** Computed locally on every machine from the replicated round end time */
	UFUNCTION(BlueprintPure, Category = "Timer")
	float GetRemainingTime() const;

	UFUNCTION(BlueprintPure, Category = "Timer")
	int32 GetRemainingSeconds() const;

	UFUNCTION(BlueprintPure, Category = "Timer")
	FText GetRemainingTimeAsText() const;

	UFUNCTION(BlueprintPure, Category = "Score")
	FText GetScoreboardText(const APlayerState* LocalPlayer) const;

	/** Fires on every machine when the round ends; RoundWinner is null on a draw */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundOver, ANoGreedyPlayerState*, RoundWinner);
	UPROPERTY(BlueprintAssignable, Category = "Winner")
	FOnRoundOver OnRoundOver;

	void SetWinner(ANoGreedyPlayerState* NewWinner);

	void CheckLastSurvivor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Leader")
	float LeaderLockDuration = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Leader")
	void RecalculateLeader();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_RoundOver();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void OnRoundTimerExpired();

	void DecideWinnerByCrystals();

	/** Server only. Stops the timer and ends the round; a null winner means a draw */
	void EndRound(ANoGreedyPlayerState* NewWinner);

	/** Server world time when the round ends; replicated once per StartRound */
	UPROPERTY(Replicated)
	float RoundEndTime = 0.f;

	/** Remaining time captured when the timer stops; negative until the first stop */
	UPROPERTY(Replicated)
	float FrozenRemainingTime = -1.f;

	FTimerHandle RoundTimerHandle;

	float LeaderLockUntil = 0.f;

	FTimerHandle LeaderLockTimerHandle;
};
