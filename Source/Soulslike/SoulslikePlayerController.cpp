// Copyright Epic Games, Inc. All Rights Reserved.


#include "SoulslikePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Soulslike.h"
#include "UI/SoulslikeHUDWidget.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ASoulslikePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASoulslikePlayerController::SetupInputComponent()
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
		}
	}
}