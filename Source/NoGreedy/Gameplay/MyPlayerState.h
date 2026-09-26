#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 *  Tracks a player's crystal count -- the single source of truth used by
 *  AGreedCharacter to compute movement speed and by AGreedGameState to
 *  work out the current leader (see "แผนที่เครือข่าย" in the design vault).
 */
UCLASS()
class AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AMyPlayerState();

	/** How many crystals this player is currently carrying. Authoritative on the server. */
	UPROPERTY(ReplicatedUsing = OnRep_CrystalCount, VisibleAnywhere, BlueprintReadOnly, Category = "Greed")
	int32 CrystalCount = 0;

	/** Server-only: adds (or removes, with a negative amount) crystals and keeps clients in sync. */
	void AddCrystals(int32 Amount);

	/** โดน AI จับแล้ว -- ตกรอบ ไม่นับเป็นคนนำอีก (server ตั้ง, replicate ให้ HUD อ่าน) */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Greed")
	bool bEliminated = false;

protected:

	UFUNCTION()
	void OnRep_CrystalCount();

	/** Called on both server and clients whenever CrystalCount changes, so speed/HUD can react. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Greed", meta = (DisplayName = "On Crystal Count Changed"))
	void BP_OnCrystalCountChanged(int32 NewCount);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
