// Copyright Epic Games, Inc. All Rights Reserved.


#include "SoulslikePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Soulslike.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ASoulslikePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASoulslikePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			for (FSL_MeleeControlStyle& eachCotnrolStyle : MeleeControlStyles) {
				if (defaultWeaponType == eachCotnrolStyle.weapon_type) {
					currentIMC = eachCotnrolStyle.IMC;
					Subsystem->AddMappingContext(eachCotnrolStyle.IMC, 1);
				}
				break;
			}
		}
	}
}

void ASoulslikePlayerController::ChangeMeleeControlStyle(ESL_WeaponType weapon_type)
{
	if (IsLocalPlayerController()) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
			Subsystem->RemoveMappingContext(currentIMC);
			
			for (FSL_MeleeControlStyle& eachCotnrolStyle : MeleeControlStyles) {
				if (weapon_type == eachCotnrolStyle.weapon_type) {
					currentIMC = eachCotnrolStyle.IMC;
					Subsystem->AddMappingContext(eachCotnrolStyle.IMC, 1);
				}
				break;
			}
		}
	}
}
