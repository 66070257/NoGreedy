#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/SlateWrapperTypes.h"
#include "NoGreedyPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class NOGREEDY_API ANoGreedyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	/** Input Mapping Contexts added only when touch controls are not used (e.g. keyboard and mouse) */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Widgets hidden while viewing through the CineCamera, restored when switching back */
	TArray<TPair<TWeakObjectPtr<UUserWidget>, ESlateVisibility>> HiddenWidgets;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Toggle Cine Camera Input Action (Bool): switches the view between the pawn and the level's CineCameraActor */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ToggleCineCameraAction;

	/** Only CineCameraActors with this tag are used; None means the first one found in the level */
	UPROPERTY(EditAnywhere, Category="Camera")
	FName CineCameraTag = NAME_None;

	/** Seconds to blend between the pawn's camera and the CineCamera */
	UPROPERTY(EditAnywhere, Category="Camera")
	float CineCameraBlendTime = 0.5f;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** A new pawn resets the view to it, so bring the UI back too */
	virtual void AcknowledgePossession(APawn* P) override;

	/** Switches the view to the CineCameraActor, or back to the pawn if already viewing it */
	void ToggleCineCamera();

	/** Hides every on-screen widget owned by this player, remembering them for ShowHiddenWidgets */
	void HideWidgets();

	/** Restores the widgets hidden by HideWidgets */
	void ShowHiddenWidgets();

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;
};
