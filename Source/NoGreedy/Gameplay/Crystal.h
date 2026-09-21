// No Greedy! game project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Crystal.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class AMyPlayerState;

/**
 *  A single crystal in the field.
 *  Server-authoritative pickup: whichever player's collision overlaps this
 *  actor first on the SERVER gets it (see "แผนที่เครือข่าย" -- no RPC needed
 *  for collecting, only for dropping).
 */
UCLASS()
class ACrystal : public AActor
{
	GENERATED_BODY()

public:

	ACrystal();

	/** Marks this crystal as just-dropped by Player, so they can't instantly re-collect it. */
	UFUNCTION(BlueprintCallable, Category = "Greed")
	void SetDroppedBy(AMyPlayerState* Player, float LockoutSeconds = 1.5f);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	/** Visual only -- no collision. Assign the crystal mesh on the Blueprint child of this class. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	/** Pickup trigger. Radius 80 per the design vault's "ตัวเลขตั้งต้น". */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollectionSphere;

	/** Purely cosmetic idle spin -- each machine spins it locally, no need to replicate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotatingMovement;

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Passes control to a Blueprint child to play pickup VFX/SFX before this actor is destroyed. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Greed", meta = (DisplayName = "On Collected"))
	void BP_OnCollected();

private:

	// server-only bookkeeping for the drop-lockout rule -- never replicated
	TWeakObjectPtr<AMyPlayerState> DroppedByPlayer;
	float RecollectAvailableTime = 0.0f;
};
