// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SoulslikePlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class USoulslikeHUDWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class ASoulslikePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Character HUD widget to spawn */
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> CharacterHUDWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** Pointer to the character HUD widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> CharacterHUDWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** HUD widget class to create at begin play */
	UPROPERTY(EditAnywhere, Category="UI|HUD")
	TSubclassOf<USoulslikeHUDWidget> HUDWidgetClass;

	/** Pointer to the active HUD widget */
	UPROPERTY(BlueprintReadOnly, Category="UI|HUD", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoulslikeHUDWidget> HUDWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

public:

	/** Returns the active HUD widget */
	UFUNCTION(BlueprintCallable, Category="UI|HUD")
	USoulslikeHUDWidget* GetHUDWidget() const { return HUDWidget; }

	/** Pushes basic vitals to HUD */
	UFUNCTION(BlueprintCallable, Category="UI|HUD")
	void SetHUDVitals(float Health, float MaxHealth, float Stamina, float MaxStamina, float Mana, float MaxMana);

};
