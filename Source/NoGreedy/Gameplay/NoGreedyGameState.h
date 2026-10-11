#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NoGreedyPlayerState.h"
#include "NoGreedyGameState.generated.h"

/**
 *  Round state shared with every client: the round timer, the current crystal leader (the AI's target) and the winner.
 *  A round ends when only one player survives, or when the timer runs out (most crystals wins, a tie is a draw).
 *  All state changes happen on the server; clients only read the replicated values.
 */
UCLASS()
class NOGREEDY_API ANoGreedyGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	bool bTimerRunning = false;

	/** Surviving player with the most crystals; the hunter AI chases this player. Null when nobody qualifies */
	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Leader", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ANoGreedyPlayerState> CurrentLeader = nullptr;

	/** Set when the round ends; stays null on a draw */
	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Winner", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ANoGreedyPlayerState> Winner = nullptr;

	/** True once the round has ended, with a winner or as a draw (Winner == nullptr) */
	UPROPERTY(ReplicatedUsing = OnRep_RoundOver, VisibleInstanceOnly, BlueprintReadOnly, Category = "Winner", meta = (AllowPrivateAccess = "true"))
	bool bRoundOver = false;

public:
	/** Round length in seconds */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	float RoundDuration = 300.f;

	/** Start the round in BeginPlay. Overwritten by the GameMode's bStartRound before BeginPlay runs */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	bool bAutoStartRound = true;

	ANoGreedyPlayerState* GetCurrentLeader() const { return CurrentLeader; }

	bool IsRoundOver() const { return bRoundOver; }

	/** Server only. Resets the winner and starts the countdown */
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StartRound();

	/** Server only. Freezes the countdown at its current value without deciding a winner */
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StopRound();

	/** Computed locally on every machine from the replicated round end time */
	UFUNCTION(BlueprintPure, Category = "Timer")
	float GetRemainingTime() const;

	UFUNCTION(BlueprintPure, Category = "Timer")
	int32 GetRemainingSeconds() const;

	UFUNCTION(BlueprintPure, Category = "Timer")
	FText GetRemainingTimeAsText() const;

	/** One line per player ("Name : Crystals"), with the local player's line wrapped in a <Me> rich text tag */
	UFUNCTION(BlueprintPure, Category = "Score")
	FText GetScoreboardText(const APlayerState* LocalPlayer) const;

	/** Fires on every machine when the round ends; RoundWinner is null on a draw */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundOver, ANoGreedyPlayerState*, RoundWinner);
	UPROPERTY(BlueprintAssignable, Category = "Winner")
	FOnRoundOver OnRoundOver;

	/** Server only. Ends the round with NewWinner; ignored when NewWinner is null (a draw only comes from the timer) */
	void SetWinner(ANoGreedyPlayerState* NewWinner);

	/** Server only. Called after each elimination; ends the round if exactly one player is left */
	void CheckLastSurvivor();

	/** Seconds a new leader is kept before the lead can pass to someone else, so the AI does not flip targets every pickup */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Leader")
	float LeaderLockDuration = 1.0f;

	/** Server only. Picks the surviving player with the most crystals. Called whenever a crystal count changes */
	UFUNCTION(BlueprintCallable, Category = "Leader")
	void RecalculateLeader();

protected:
	virtual void BeginPlay() override;

	/** Broadcasts OnRoundOver. Runs on clients via replication and is called directly on the server */
	UFUNCTION()
	void OnRep_RoundOver();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void OnRoundTimerExpired();

	/** Server only. Time ran out: the surviving player with the most crystals wins; a tie for first is a draw */
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

	/** Server only. World time until which CurrentLeader cannot be replaced (unless they are eliminated) */
	float LeaderLockUntil = 0.f;

	/** Server only. Re-runs RecalculateLeader when the lock expires */
	FTimerHandle LeaderLockTimerHandle;
};
