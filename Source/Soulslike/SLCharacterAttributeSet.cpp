// Fill out your copyright notice in the Description page of Project Settings.


#include "SLCharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Weapons/SLWeaponTypes.h"
#include "Net/UnrealNetwork.h"
#include "Soulslike.h"
#include "SoulslikeCharacter.h"

namespace
{
	constexpr float StaminaRegenResumeDelay = 1.2f;
}

USLCharacterAttributeSet::USLCharacterAttributeSet() :
	MaxHealth(100.f),
	MaxStamina(100.f),
	MaxMana(40.f),
	MaxPoise(0.f),
	MaxPower(0.f),
	MaxGroggy(100.f),
	MaxLevel(99.f)
{
	InitHealth(GetMaxHealth());
	InitStamina(GetMaxStamina());
	InitMana(GetMaxMana());
	InitPoise(GetMaxPoise());
	InitPower(GetMaxPower());
	InitGroggy(GetMaxGroggy());
	InitLevel(GetMaxLevel());
}

void USLCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, MaxPoise, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, Poise, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, MaxPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, Power, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, MaxGroggy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, Groggy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, MaxLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USLCharacterAttributeSet, Damage, COND_None, REPNOTIFY_Always);
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
	else if (Attribute == GetMaxGroggyAttribute())
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
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetPoiseAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxPoise());
	}
	else if (Attribute == GetPowerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxPower());
	}
	else if (Attribute == GetGroggyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxGroggy());
	}
	else if (Attribute == GetLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxLevel());
	}
}

void USLCharacterAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetGroggyAttribute())
	{
		SetGroggy(FMath::Clamp(GetGroggy(), 0.f, GetMaxGroggy()));
	}

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
		const FGameplayTag GroggyTag = FGameplayTag::RequestGameplayTag(SLCombatTags::State_Groggy, /*ErrorIfNotFound*/ false);
		const FGameplayTag PoiseDamageTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_PoiseDamage, /*ErrorIfNotFound*/ false);

		// Already dead — ignore further damage so we don't fire the death event twice.
		if (TargetASC && DeadTag.IsValid() && TargetASC->HasMatchingGameplayTag(DeadTag))
		{
			return;
		}

		const float NewHealth = FMath::Clamp(GetHealth() - Incoming, 0.f, GetMaxHealth());
		SetHealth(NewHealth);

		// Consume optional poise damage as groggy damage.
		if (TargetASC && PoiseDamageTag.IsValid())
		{
			const float IncomingPoiseDamage = Data.EffectSpec.GetSetByCallerMagnitude(PoiseDamageTag, false, 0.0f);
			if (IncomingPoiseDamage > 0.0f && GetMaxGroggy() > 0.0f)
			{
				const float NewGroggy = FMath::Clamp(GetGroggy() - IncomingPoiseDamage, 0.0f, GetMaxGroggy());
				SetGroggy(NewGroggy);

				if (NewGroggy <= 0.0f && GroggyTag.IsValid() && !TargetASC->HasMatchingGameplayTag(GroggyTag))
				{
					TargetASC->AddLooseGameplayTag(GroggyTag);
				}
			}
		}

		if (TargetASC) {
			ASoulslikeCharacter* SLChar = Cast<ASoulslikeCharacter>(TargetASC->GetAvatarActor());
			SLChar->DoOnHit();
		}

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
					SLDEBUG("send death event to actor = %s", *Avatar->GetName());
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Avatar, DeathEvent, Payload);
				}
			}
		}
	}
}

void USLCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, MaxHealth, OldValue); }
void USLCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, Health, OldValue); }
void USLCharacterAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, MaxStamina, OldValue); }
void USLCharacterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, Stamina, OldValue); }
void USLCharacterAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, MaxMana, OldValue); }
void USLCharacterAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, Mana, OldValue); }
void USLCharacterAttributeSet::OnRep_MaxPoise(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, MaxPoise, OldValue); }
void USLCharacterAttributeSet::OnRep_Poise(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, Poise, OldValue); }
void USLCharacterAttributeSet::OnRep_MaxPower(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, MaxPower, OldValue); }
void USLCharacterAttributeSet::OnRep_Power(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, Power, OldValue); }
void USLCharacterAttributeSet::OnRep_MaxGroggy(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, MaxGroggy, OldValue); }
void USLCharacterAttributeSet::OnRep_Groggy(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, Groggy, OldValue); }
void USLCharacterAttributeSet::OnRep_MaxLevel(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, MaxLevel, OldValue); }
void USLCharacterAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, Level, OldValue); }
void USLCharacterAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(USLCharacterAttributeSet, Damage, OldValue); }
