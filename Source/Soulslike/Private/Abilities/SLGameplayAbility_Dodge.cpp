#include "Abilities/SLGameplayAbility_Dodge.h"

#include "Abilities/SLGE_StaminaCost.h"
#include "SLCharacterAttributeSet.h"
#include "Weapons/SLWeaponTypes.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

USLGameplayAbility_Dodge::USLGameplayAbility_Dodge()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	StaminaCostGEClass = USLGE_StaminaCost::StaticClass();
	
	const FGameplayTag DodgingTag = FGameplayTag::RequestGameplayTag(SLCombatTags::State_Dodging, /*ErrorIfNotFound*/ false);
	const FGameplayTag InvulnTag = FGameplayTag::RequestGameplayTag(SLCombatTags::State_Invulnerable, /*ErrorIfNotFound*/ false);
	if (DodgingTag.IsValid())
	{
		ActivationOwnedTags.AddTag(DodgingTag);
	}
	if (InvulnTag.IsValid())
	{ 
		ActivationOwnedTags.AddTag(InvulnTag);
	}

	const FGameplayTag ActivateTag = FGameplayTag::RequestGameplayTag(SLCombatTags::Activate_Dodge, /*ErrorIfNotFound*/ false);
	if (ActivateTag.IsValid())
	{
		FGameplayTagContainer AssetTags;
		AssetTags.AddTag(ActivateTag);
		SetAssetTags(AssetTags);
	}
	
}

bool USLGameplayAbility_Dodge::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	// Block dodge if there isn't enough stamina available.
	if (ActorInfo)
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			if (ASC->GetNumericAttribute(USLCharacterAttributeSet::GetStaminaAttribute()) < StaminaCost)
			{
				return false;
			}
		}
	}

	return true;
}

void USLGameplayAbility_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEnd*/ true, /*bWasCancelled*/ true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEnd*/ true, /*bWasCancelled*/ true);
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	// Apply stamina cost via SetByCaller. We pass a negative number so the
	// additive modifier inside USLGE_StaminaCost subtracts from Stamina.
	if (StaminaCostGEClass)
	{
		FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
		Ctx.AddSourceObject(ActorInfo->AvatarActor.Get());
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StaminaCostGEClass, /*Level*/ GetAbilityLevel(), Ctx);
		if (SpecHandle.IsValid())
		{
			const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_StaminaCost, /*ErrorIfNotFound*/ false);
			if (CostTag.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(CostTag, -StaminaCost);
			}
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	// Play the dodge montage and end the ability when it finishes.
	if (DodgeMontage)
	{
		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		if (Character && Character->GetMesh())
		{
			if (UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance())
			{
				const float Duration = AnimInst->Montage_Play(DodgeMontage, 1.f);
				if (Duration > 0.f)
				{
					FOnMontageEnded EndedDelegate;
					EndedDelegate.BindUObject(this, &USLGameplayAbility_Dodge::OnDodgeMontageEnded);
					AnimInst->Montage_SetEndDelegate(EndedDelegate, DodgeMontage);
					return; // Stay active until OnDodgeMontageEnded fires.
				}
			}
		}
	}

	// No montage path — end immediately so we don't get stuck active.
	EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEnd*/ true, /*bWasCancelled*/ false);
}

void USLGameplayAbility_Dodge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// If we were cancelled mid-roll, stop the montage so the character returns to idle.
	if (bWasCancelled && DodgeMontage && ActorInfo)
	{
		if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (Character->GetMesh())
			{
				if (UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance())
				{
					AnimInst->Montage_Stop(0.15f, DodgeMontage);
				}
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USLGameplayAbility_Dodge::OnDodgeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != DodgeMontage) { return; }
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), /*bReplicateEnd*/ true, /*bWasCancelled*/ bInterrupted);
}
