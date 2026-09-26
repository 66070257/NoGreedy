#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyPlayerState.h"
#include "MyGameState.generated.h"

UCLASS()
class NOGREEDY_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMyGameState();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	float RoundDuration = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	bool bAutoStartRound = true;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, VisibleInstanceOnly, BlueprintReadOnly, Category = "Timer")
	float RemainingTime = 300.f;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Timer")
	bool bTimerRunning = false;

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StartRound();

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StopRound();

	UFUNCTION(BlueprintPure, Category = "Timer")
	int32 GetRemainingSeconds() const;

	UFUNCTION(BlueprintPure, Category = "Timer")
	FString GetRemainingTimeAsString() const;

	UFUNCTION(BlueprintPure, Category = "Timer")
	FText GetRemainingTimeAsText() const;

	UFUNCTION(BlueprintPure, Category = "Score")
	FText GetScoreboardText(const APlayerState* LocalPlayer) const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingTimeChanged, float, NewRemainingTime);
	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FOnRemainingTimeChanged OnRemainingTimeChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundTimeUp);
	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FOnRoundTimeUp OnRoundTimeUp;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentLeader, VisibleInstanceOnly, BlueprintReadOnly, Category = "Leader")
	AMyPlayerState* CurrentLeader = nullptr;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Winner")
	AMyPlayerState* Winner = nullptr;

	void SetWinner(AMyPlayerState* NewWinner);

	void CheckLastSurvivor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Leader")
	float LeaderLockDuration = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Leader")
	void RecalculateLeader();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeaderChanged, AMyPlayerState*, NewLeader);
	UPROPERTY(BlueprintAssignable, Category = "Leader")
	FOnLeaderChanged OnLeaderChanged;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_CurrentLeader();

	UFUNCTION()
	void OnRep_RemainingTime();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void HandleRemainingTimeChanged();

	void DecideWinnerByCrystals();

	bool bHasFiredTimeUp = false;

	float LeaderLockUntil = 0.f;
};
