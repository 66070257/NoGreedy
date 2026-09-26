#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NoGreedyGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class NOGREEDY_API ANoGreedyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Rules")
	void TravelToGame();

	/** Server only. bDropCrystals = false means the victim's crystals are lost (e.g. fell into a pit) */
	void EliminatePlayer(AController* Victim, AController* Killer, bool bDropCrystals = true);

protected:
	virtual void BeginPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UFUNCTION()
	void HandleRoundOver(class ANoGreedyPlayerState* RoundWinner);

	/** Seconds after the round ends before restarting the Game map; 0 or less disables it */
	UPROPERTY(EditDefaultsOnly, Category="Rules")
	float RestartDelay = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Rules")
	FName PlayerSpawnTag = TEXT("PlayerSpawn");

	UPROPERTY(EditDefaultsOnly, Category="Rules")
	TSubclassOf<class ACrystal> CrystalClass;

	UPROPERTY(EditDefaultsOnly, Category="Rules")
	float ScatterRadius = 300.f;

private:
	FTimerHandle RestartTimerHandle;
};

