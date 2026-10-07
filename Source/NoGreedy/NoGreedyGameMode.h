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

	/** Server only. Takes one crystal from the player and spawns it behind their pawn. Returns false if nothing was dropped */
	bool DropCrystal(AController* Dropper);

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

	/** How far behind the player a dropped crystal lands */
	UPROPERTY(EditDefaultsOnly, Category="Rules")
	float DropDistance = 150.f;

	/** Seconds before the dropper can pick their own crystal back up */
	UPROPERTY(EditDefaultsOnly, Category="Rules")
	float DropRecollectLockout = 1.5f;

private:
	FTimerHandle RestartTimerHandle;
};

