// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLCharacterHUDWidget.generated.h"

class UAbilitySystemComponent;
class USLCharacterAttributeSet;
class ABoss;

/**
 * Basic character HUD base for Health / Stamina / Mana.
 * Create a Blueprint from this class and implement UpdateStatusBars.
 */
UCLASS(Abstract, Blueprintable)
class SOULSLIKE_API USLCharacterHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Force-refresh current resource values. */
	UFUNCTION(BlueprintCallable, Category="HUD")
	void RefreshStatusBars();

	/** Force-refresh boss header data (name / hp). */
	UFUNCTION(BlueprintCallable, Category="HUD")
	void RefreshBossStatus();

protected:
	/** Implement in BP to drive ProgressBars/Text. All percentages are normalized [0..1]. */
	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void UpdateStatusBars(float HealthPercent, float StaminaPercent, float ManaPercent, float Health, float MaxHealth, float Stamina, float MaxStamina, float Mana, float MaxMana);

	/** Implement in BP to drive top boss UI. */
	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void UpdateBossStatus(bool bVisible, const FText& BossName, float BossHealthPercent, float BossHealth, float BossMaxHealth);

private:
	bool ResolveAttributeSources();
	ABoss* ResolveTrackedBoss();
	float SafePercent(float Current, float Max) const;
	bool IsBossInHudRange(const ABoss* Boss) const;

	UPROPERTY(EditAnywhere, Category="HUD|Boss", meta=(ClampMin="0.0"))
	float BossHudVisibleDistance = 2500.0f;

	TWeakObjectPtr<UAbilitySystemComponent> CachedAbilitySystem;
	const USLCharacterAttributeSet* CachedAttributeSet = nullptr;
	TWeakObjectPtr<ABoss> CachedBoss;
};
