#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NoGreedyFreeCamera.generated.h"

class UCineCameraComponent;
class UFloatingPawnMovement;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 *  Free-flying cinematic camera pawn. Flies in any direction with no gravity or collision.
 *  Adds its own mapping context when possessed, so it works with any PlayerController.
 *  Meant to be the Default Pawn of a separate GameMode for flythroughs and recording clips.
 */
UCLASS()
class NOGREEDY_API ANoGreedyFreeCamera : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCineCameraComponent> CineCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFloatingPawnMovement> FlyMovement;

protected:

	/** Mapping context added while this pawn is possessed */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> FlyMappingContext;

	/** Move Input Action (Axis2D: X = right, Y = forward) */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	/** Look Input Action (Axis2D: X = yaw, Y = pitch) */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	/** Fly Up/Down Input Action (Axis1D: + = up, - = down) */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> FlyUpAction;

	/** Boost Input Action (Bool, held) */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> BoostAction;

	/** Adjust Speed Input Action (Axis1D, Mouse Wheel Axis: + = faster, - = slower) */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AdjustSpeedAction;

	/** Normal flying speed in cm/s */
	UPROPERTY(EditAnywhere, Category = "Fly")
	float FlySpeed = 1200.f;

	/** FlySpeed is multiplied by this while Boost is held */
	UPROPERTY(EditAnywhere, Category = "Fly")
	float BoostMultiplier = 3.f;

	/** FlySpeed is multiplied (wheel up) or divided (wheel down) by this per wheel notch */
	UPROPERTY(EditAnywhere, Category = "Fly", meta = (ClampMin = "1.01"))
	float SpeedStepMultiplier = 1.2f;

	/** Mouse wheel can't take FlySpeed below this */
	UPROPERTY(EditAnywhere, Category = "Fly")
	float MinFlySpeed = 100.f;

	/** Mouse wheel can't take FlySpeed above this */
	UPROPERTY(EditAnywhere, Category = "Fly")
	float MaxFlySpeed = 20000.f;

	/** True while Boost is held */
	bool bBoosting = false;

public:
	ANoGreedyFreeCamera();

protected:
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void FlyUp(const FInputActionValue& Value);
	void BoostStart();
	void BoostStop();
	void AdjustSpeed(const FInputActionValue& Value);

	/** Pushes FlySpeed (and boost, if held) into the movement component */
	void ApplyFlySpeed();
};
