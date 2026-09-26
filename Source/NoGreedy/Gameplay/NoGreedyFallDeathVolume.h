#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoGreedyFallDeathVolume.generated.h"

class UBoxComponent;

/**
 *  Place under a pit: any player whose pawn falls into the box is eliminated.
 *  Their crystals are lost instead of being dropped where nobody can reach them.
 */
UCLASS()
class NOGREEDY_API ANoGreedyFallDeathVolume : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> KillBox;

public:
	ANoGreedyFallDeathVolume();

protected:
	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
