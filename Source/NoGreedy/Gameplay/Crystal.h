#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Crystal.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class AMyPlayerState;

UCLASS()
class ACrystal : public AActor
{
	GENERATED_BODY()

public:
	ACrystal();

	UFUNCTION(BlueprintCallable, Category = "Greed")
	void SetDroppedBy(AMyPlayerState* Player, float LockoutSeconds = 1.5f);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollectionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotatingMovement;

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Greed", meta = (DisplayName = "On Collected"))
	void BP_OnCollected();

private:
	TWeakObjectPtr<AMyPlayerState> DroppedByPlayer;
	float RecollectAvailableTime = 0.0f;
};
