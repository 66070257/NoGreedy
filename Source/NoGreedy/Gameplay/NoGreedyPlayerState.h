#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NoGreedyPlayerState.generated.h"

UCLASS()
class NOGREEDY_API ANoGreedyPlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing = OnRep_CrystalCount, VisibleAnywhere, BlueprintReadOnly, Category = "Greed", meta = (AllowPrivateAccess = "true"))
	int32 CrystalCount = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Greed", meta = (AllowPrivateAccess = "true"))
	bool bEliminated = false;

public:
	int32 GetCrystalCount() const { return CrystalCount; }

	bool IsEliminated() const { return bEliminated; }

	/** Server only. Returns false if the change was rejected (client, eliminated player or round over) */
	bool AddCrystals(int32 Amount);

	/** Server only. Marks this player eliminated, clears their crystals and returns how many they held */
	int32 Eliminate();

protected:
	UFUNCTION()
	void OnRep_CrystalCount();

	UFUNCTION(BlueprintImplementableEvent, Category = "Greed", meta = (DisplayName = "On Crystal Count Changed"))
	void BP_OnCrystalCountChanged(int32 NewCount);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
