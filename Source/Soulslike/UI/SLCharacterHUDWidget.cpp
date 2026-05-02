// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SLCharacterHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "SLCharacterAttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

void USLCharacterHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshStatusBars();
}

void USLCharacterHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshStatusBars();
}

void USLCharacterHUDWidget::RefreshStatusBars()
{
	if (!ResolveAttributeSources() || !CachedAttributeSet)
	{
		UpdateStatusBars(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		return;
	}

	const float Health = CachedAttributeSet->GetHealth();
	const float MaxHealth = CachedAttributeSet->GetMaxHealth();
	const float Stamina = CachedAttributeSet->GetStamina();
	const float MaxStamina = CachedAttributeSet->GetMaxStamina();
	const float Mana = CachedAttributeSet->GetMana();
	const float MaxMana = CachedAttributeSet->GetMaxMana();

	UpdateStatusBars(
		SafePercent(Health, MaxHealth),
		SafePercent(Stamina, MaxStamina),
		SafePercent(Mana, MaxMana),
		Health,
		MaxHealth,
		Stamina,
		MaxStamina,
		Mana,
		MaxMana);
}

bool USLCharacterHUDWidget::ResolveAttributeSources()
{
	if (CachedAbilitySystem.IsValid() && CachedAttributeSet)
	{
		return true;
	}

	APlayerController* OwnerPC = GetOwningPlayer();
	if (!OwnerPC)
	{
		return false;
	}

	APlayerState* OwnerPS = OwnerPC->GetPlayerState<APlayerState>();
	if (!OwnerPS)
	{
		return false;
	}

	IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(OwnerPS);
	if (!AbilityInterface)
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystem = AbilityInterface->GetAbilitySystemComponent();
	if (!AbilitySystem)
	{
		return false;
	}

	CachedAbilitySystem = AbilitySystem;
	CachedAttributeSet = AbilitySystem->GetSet<USLCharacterAttributeSet>();
	return CachedAttributeSet != nullptr;
}

float USLCharacterHUDWidget::SafePercent(float Current, float Max) const
{
	if (Max <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(Current / Max, 0.0f, 1.0f);
}
