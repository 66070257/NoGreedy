#include "NoGreedyFreeCamera.h"
#include "CineCameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

ANoGreedyFreeCamera::ANoGreedyFreeCamera()
{
	PrimaryActorTick.bCanEverTick = false;

	CineCamera = CreateDefaultSubobject<UCineCameraComponent>(TEXT("CineCamera"));
	RootComponent = CineCamera;

	FlyMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FlyMovement"));
	FlyMovement->SetUpdatedComponent(CineCamera);

	// Camera follows the controller rotation on both axes so mouse look turns the view
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
}

void ANoGreedyFreeCamera::BeginPlay()
{
	Super::BeginPlay();

	// Apply FlySpeed here so values edited in the Blueprint take effect
	ApplyFlySpeed();
}

void ANoGreedyFreeCamera::NotifyControllerChanged()
{
	const APlayerController* PreviousPC = Cast<APlayerController>(PreviousController);
	if (PreviousPC && FlyMappingContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPC->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FlyMappingContext);
		}
	}

	if (const APlayerController* PC = Cast<APlayerController>(Controller); PC && FlyMappingContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FlyMappingContext, 1);
		}
	}

	Super::NotifyControllerChanged();
}

void ANoGreedyFreeCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANoGreedyFreeCamera::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANoGreedyFreeCamera::Look);
		EnhancedInputComponent->BindAction(FlyUpAction, ETriggerEvent::Triggered, this, &ANoGreedyFreeCamera::FlyUp);
		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Started, this, &ANoGreedyFreeCamera::BoostStart);
		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Completed, this, &ANoGreedyFreeCamera::BoostStop);
		EnhancedInputComponent->BindAction(AdjustSpeedAction, ETriggerEvent::Triggered, this, &ANoGreedyFreeCamera::AdjustSpeed);
	}
}

void ANoGreedyFreeCamera::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (GetController() != nullptr)
	{
		// Use the full control rotation (pitch included) so W flies toward wherever the camera looks
		const FRotationMatrix ControlMatrix(GetController()->GetControlRotation());

		AddMovementInput(ControlMatrix.GetUnitAxis(EAxis::X), MovementVector.Y);
		AddMovementInput(ControlMatrix.GetUnitAxis(EAxis::Y), MovementVector.X);
	}
}

void ANoGreedyFreeCamera::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (GetController() != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ANoGreedyFreeCamera::FlyUp(const FInputActionValue& Value)
{
	// World up, so E/Q always go straight up/down regardless of camera tilt
	AddMovementInput(FVector::UpVector, Value.Get<float>());
}

void ANoGreedyFreeCamera::BoostStart()
{
	bBoosting = true;
	ApplyFlySpeed();
}

void ANoGreedyFreeCamera::BoostStop()
{
	bBoosting = false;
	ApplyFlySpeed();
}

void ANoGreedyFreeCamera::AdjustSpeed(const FInputActionValue& Value)
{
	const float WheelDelta = Value.Get<float>();

	// Multiplicative steps so the wheel feels the same at low and high speeds
	FlySpeed = FMath::Clamp(FlySpeed * FMath::Pow(SpeedStepMultiplier, WheelDelta), MinFlySpeed, MaxFlySpeed);
	ApplyFlySpeed();
}

void ANoGreedyFreeCamera::ApplyFlySpeed()
{
	FlyMovement->MaxSpeed = bBoosting ? FlySpeed * BoostMultiplier : FlySpeed;
}
