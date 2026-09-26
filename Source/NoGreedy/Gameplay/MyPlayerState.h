#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

UCLASS()
class AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMyPlayerState();

	UPROPERTY(ReplicatedUsing = OnRep_CrystalCount, VisibleAnywhere, BlueprintReadOnly, Category = "Greed")
	int32 CrystalCount = 0;

	void AddCrystals(int32 Amount);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Greed")
	bool bEliminated = false;

protected:
	UFUNCTION()
	void OnRep_CrystalCount();

	UFUNCTION(BlueprintImplementableEvent, Category = "Greed", meta = (DisplayName = "On Crystal Count Changed"))
	void BP_OnCrystalCountChanged(int32 NewCount);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
