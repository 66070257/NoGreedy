#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NoGreedyGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ANoGreedyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	ANoGreedyGameMode();

	UFUNCTION(BlueprintCallable, Category="Rules")
	void TravelToGame();

	void EliminatePlayer(AController* Victim, AController* Killer);

protected:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly, Category="Rules")
	FName PlayerSpawnTag = TEXT("PlayerSpawn");

	UPROPERTY(EditDefaultsOnly, Category="Rules")
	TSubclassOf<class ACrystal> CrystalClass;

	UPROPERTY(EditDefaultsOnly, Category="Rules")
	float ScatterRadius = 300.f;
};

