#include "NoGreedyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NoGreedyGameMode.h"
#include "Gameplay/NoGreedyPlayerState.h"

ANoGreedyCharacter::ANoGreedyCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void ANoGreedyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANoGreedyCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ANoGreedyCharacter::Look);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANoGreedyCharacter::Look);

		if (DropAction)
		{
			EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &ANoGreedyCharacter::DoDropCrystal);
		}
	}
}

void ANoGreedyCharacter::FellOutOfWorld(const UDamageType& DmgType)
{
	if (HasAuthority())
	{
		if (ANoGreedyGameMode* GM = GetWorld()->GetAuthGameMode<ANoGreedyGameMode>())
		{
			GM->EliminatePlayer(GetController(), nullptr, false);
		}
	}

	// EliminatePlayer destroys the pawn; otherwise (e.g. round already over) fall back to the default cleanup
	if (!IsActorBeingDestroyed())
	{
		Super::FellOutOfWorld(DmgType);
	}
}

void ANoGreedyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UpdateSpeedFromPlayerState();
}

void ANoGreedyCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	UpdateSpeedFromPlayerState();
}

void ANoGreedyCharacter::UpdateSpeedFromPlayerState()
{
	const ANoGreedyPlayerState* PS = GetPlayerState<ANoGreedyPlayerState>();
	UpdateSpeedFromCrystals(PS ? PS->GetCrystalCount() : 0);
}

void ANoGreedyCharacter::UpdateSpeedFromCrystals(int32 CrystalCount)
{
	GetCharacterMovement()->MaxWalkSpeed = FMath::Max(BaseSpeed - SpeedLossPerCrystal * CrystalCount, MinSpeed);
}

void ANoGreedyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	DoMove(MovementVector.X, MovementVector.Y);
}

void ANoGreedyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ANoGreedyCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ANoGreedyCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ANoGreedyCharacter::DoJumpStart()
{
	Jump();
}

void ANoGreedyCharacter::DoJumpEnd()
{
	StopJumping();
}

void ANoGreedyCharacter::DoDropCrystal()
{
	ServerDropCrystal();
}

void ANoGreedyCharacter::ServerDropCrystal_Implementation()
{
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastDropTime < DropCooldown)
	{
		return;
	}

	ANoGreedyGameMode* GM = GetWorld()->GetAuthGameMode<ANoGreedyGameMode>();
	if (GM && GM->DropCrystal(GetController()))
	{
		LastDropTime = Now;
	}
}

void ANoGreedyCharacter::MulticastPlayCrystalFX_Implementation(FVector_NetQuantize Location, bool bCollected)
{
	UNiagaraSystem* Effect = bCollected ? CollectEffect : DropEffect;
	USoundBase* Sound = bCollected ? CollectSound : DropSound;

	if (Effect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Effect, Location);
	}

	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, Location);
	}
}
