// Fill out your copyright notice in the Description page of Project Settings.


#include "SLCharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Weapons/SLWeaponTypes.h"

namespace
{
	constexpr float StaminaRegenResumeDelay = 0.8f;
}

USLCharacterAttributeSet::USLCharacterAttributeSet() :
	MaxHealth(100.f),
	MaxStamina(100.f),
	MaxMana(40.f),
	MaxPoise(0.f),
	MaxPower(0.f),
	MaxLevel(99.f)
{
	InitHealth(GetMaxHealth());
	InitStamina(GetMaxStamina());
	InitMana(GetMaxMana());
	InitPoise(GetMaxPoise());
	InitPower(GetMaxPower());
	InitLevel(GetMaxLevel());
}

void USLCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Clamp Max Values to be always positive 
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(1, NewValue);
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(1, NewValue);
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(1, NewValue);
	}
	else if (Attribute == GetMaxPoiseAttribute())
	{
		NewValue = FMath::Max(1, NewValue);
	}
	else if (Attribute == GetMaxPowerAttribute())
	{
		NewValue = FMath::Max(1, NewValue);
	}
	else if (Attribute == GetMaxLevelAttribute())
	{
		NewValue = FMath::Max(1, NewValue);
	}
	// Clamp Values to be always [0~MaxValue]
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxStamina());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxMana());
	}
	else if (Attribute == GetPoiseAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxPoise());
	}
	else if (Attribute == GetPowerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxPower());
	}
	else if (Attribute == GetLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxLevel());
	}
}

void USLCharacterAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	// Stamina spend → set State.Stamina.Spending and (re)arm a timer to clear it.
	// USLGE_StaminaRegen ignores ticks while the tag is present, which produces
	// the "regen pauses during spend, resumes shortly after" behavior.
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute() && Data.EvaluatedData.Magnitude < 0.f)
	{
		UAbilitySystemComponent* TargetASC = &Data.Target;
		const FGameplayTag SpendingTag = FGameplayTag::RequestGameplayTag(SLCombatTags::State_StaminaSpending, /*ErrorIfNotFound*/ false);
		if (TargetASC && SpendingTag.IsValid())
		{
			// Use SetLooseGameplayTagCount instead of AddLooseGameplayTag so repeated
			// spends don't stack a counter we'd then need to drain.
			TargetASC->SetLooseGameplayTagCount(SpendingTag, 1);

			if (AActor* Owner = TargetASC->GetOwnerActor())
			{
				if (UWorld* World = Owner->GetWorld())
				{
					TWeakObjectPtr<UAbilitySystemComponent> WeakASC(TargetASC);
					World->GetTimerManager().ClearTimer(StaminaSpendingClearTimer);
					World->GetTimerManager().SetTimer(
						StaminaSpendingClearTimer,
						[WeakASC, SpendingTag]()
						{
							if (UAbilitySystemComponent* ASC = WeakASC.Get())
							{
								ASC->SetLooseGameplayTagCount(SpendingTag, 0);
							}
						},
						StaminaRegenResumeDelay,
						/*bLoop*/ false);
				}
			}
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float Incoming = GetDamage();
		SetDamage(0.f);

		UAbilitySystemComponent* TargetASC = &Data.Target;
		const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(SLCombatTags::State_Dead, /*ErrorIfNotFound*/ false);

		// Already dead — ignore further damage so we don't fire the death event twice.
		if (TargetASC && DeadTag.IsValid() && TargetASC->HasMatchingGameplayTag(DeadTag))
		{
			return;
		}

		const float NewHealth = FMath::Clamp(GetHealth() - Incoming, 0.f, GetMaxHealth());
		SetHealth(NewHealth);

		// Lethal hit: mark dead + broadcast a death gameplay event so anim / AI /
		// game mode listeners can react. We add State.Dead as a loose tag so it
		// persists past the instant GE that killed the target.
		if (NewHealth <= 0.f && TargetASC)
		{
			if (DeadTag.IsValid())
			{
				TargetASC->AddLooseGameplayTag(DeadTag);
			}

			// Cancel anything the dying actor was casting — no half-finished swings.
			TargetASC->CancelAllAbilities();

			const FGameplayTag DeathEvent = FGameplayTag::RequestGameplayTag(SLCombatTags::Event_Death, /*ErrorIfNotFound*/ false);
			if (DeathEvent.IsValid())
			{
				FGameplayEventData Payload;
				Payload.EventTag = DeathEvent;
				Payload.Target = Data.Target.GetAvatarActor();
				if (Data.EffectSpec.GetContext().GetInstigator())
				{
					Payload.Instigator = Data.EffectSpec.GetContext().GetInstigator();
				}
				Payload.ContextHandle = Data.EffectSpec.GetContext();
				Payload.EventMagnitude = Incoming;

				if (AActor* Avatar = Data.Target.GetAvatarActor())
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Avatar, DeathEvent, Payload);
				}
			}
		}
	}
}
