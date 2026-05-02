// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoulslikeHUDWidget.generated.h"

/**
 * Base in-game HUD widget.
 * Implement visual updates in Blueprint.
 */
UCLASS(Abstract)
class SOULSLIKE_API USoulslikeHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void UpdateHealth(float Current, float Max);

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void UpdateStamina(float Current, float Max);

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void UpdateMana(float Current, float Max);

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void SetInteractionHint(const FText& HintText, bool bVisible);
};

