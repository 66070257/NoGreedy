#include "NoGreedyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "CineCameraActor.h"
#include "EngineUtils.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ANoGreedyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			MobileControlsWidget->AddToPlayerScreen(0);
		}
	}
}

void ANoGreedyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(ToggleCineCameraAction, ETriggerEvent::Started, this, &ANoGreedyPlayerController::ToggleCineCamera);
		}
	}
}

void ANoGreedyPlayerController::ToggleCineCamera()
{
	// Already looking through a CineCamera: go back to the pawn
	if (GetViewTarget() && GetViewTarget()->IsA<ACineCameraActor>())
	{
		if (APawn* MyPawn = GetPawn())
		{
			SetViewTargetWithBlend(MyPawn, CineCameraBlendTime);
		}
		ShowHiddenWidgets();
		return;
	}

	for (TActorIterator<ACineCameraActor> It(GetWorld()); It; ++It)
	{
		if (CineCameraTag.IsNone() || It->ActorHasTag(CineCameraTag))
		{
			SetViewTargetWithBlend(*It, CineCameraBlendTime);
			HideWidgets();
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("NoGreedy: no CineCameraActor found in the level to switch to"));
}

bool ANoGreedyPlayerController::ShouldUseTouchControls() const
{
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void ANoGreedyPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	ShowHiddenWidgets();
}

void ANoGreedyPlayerController::HideWidgets()
{
	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, Widgets, UUserWidget::StaticClass(), true);

	for (UUserWidget* Widget : Widgets)
	{
		// Only this player's widgets that are actually showing, so split-screen players keep theirs
		if (Widget->GetOwningPlayer() == this && Widget->IsVisible())
		{
			HiddenWidgets.Emplace(Widget, Widget->GetVisibility());
			Widget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ANoGreedyPlayerController::ShowHiddenWidgets()
{
	for (const TPair<TWeakObjectPtr<UUserWidget>, ESlateVisibility>& Hidden : HiddenWidgets)
	{
		if (UUserWidget* Widget = Hidden.Key.Get())
		{
			Widget->SetVisibility(Hidden.Value);
		}
	}

	HiddenWidgets.Reset();
}
