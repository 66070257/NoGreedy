#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Crystal.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class ANoGreedyPlayerState;

/**
 *  Replicated pickup. A player-controlled character that overlaps it gains one crystal and the actor is destroyed.
 *  Spawned by the GameMode when a player drops one or is eliminated; can also be placed in the level.
 */
UCLASS()
class NOGREEDY_API ACrystal : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Overlaps pawns only; this is what triggers collection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> CollectionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URotatingMovementComponent> RotatingMovement;

public:
	ACrystal();

	/** Stops the dropping player from re-collecting this crystal for LockoutSeconds */
	UFUNCTION(BlueprintCallable, Category = "Greed")
	void SetDroppedBy(ANoGreedyPlayerState* Player, float LockoutSeconds = 1.5f);

protected:
	/** Server only. Gives the crystal to the overlapping player, then destroys this actor */
	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:
	/** Server only. Player who dropped this crystal; unset for crystals placed in the level or scattered on elimination */
	TWeakObjectPtr<ANoGreedyPlayerState> DroppedByPlayer;

	/** Server only. World time after which DroppedByPlayer may collect this crystal again */
	float RecollectAvailableTime = 0.0f;
};
