#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Crystal.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class ANoGreedyPlayerState;

UCLASS()
class NOGREEDY_API ACrystal : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

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
	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:
	TWeakObjectPtr<ANoGreedyPlayerState> DroppedByPlayer;
	float RecollectAvailableTime = 0.0f;
};
