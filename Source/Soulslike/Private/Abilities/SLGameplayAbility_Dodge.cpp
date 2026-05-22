#include "Abilities/SLGameplayAbility_Dodge.h"

#include "SLCharacterAttributeSet.h"
#include "Weapons/SLWeaponTypes.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

USLGameplayAbility_Dodge::USLGameplayAbility_Dodge()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
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
		FGameplayTagContainer Tags = GetAssetTags();
		Tags.AddTag(ActivateTag);
		SetAssetTags(Tags);
	}
	
}

bool USLGameplayAbility_Dodge::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	if (ActorInfo)
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			const float CurrentStamina = ASC->GetNumericAttribute(USLCharacterAttributeSet::GetStaminaAttribute());
			if (CurrentStamina < StaminaCost)
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
       EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
       return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
       EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
       return;
    }

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

    // --- Apply Stamina Cost (Your original working logic) ---
    if (StaminaCostGEClass)
    {
       FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
       Ctx.AddSourceObject(ActorInfo->AvatarActor.Get());
       FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StaminaCostGEClass, GetAbilityLevel(), Ctx);
       if (SpecHandle.IsValid())
       {
          const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_StaminaCost, false);
          if (CostTag.IsValid())
          {
             SpecHandle.Data->SetSetByCallerMagnitude(CostTag, -StaminaCost);
          }
          ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
       }
    }

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    
	ActiveDodgeMontage = SelectDodgeMontage(Character);

	if (ActiveDodgeMontage && Character && Character->GetMesh())
	{
		if (UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance())
		{
			const float Duration = AnimInst->Montage_Play(ActiveDodgeMontage, DodgeSpeedMultiplier);
			if (Duration > 0.f)
			{
				FOnMontageEnded EndedDelegate;
				EndedDelegate.BindUObject(this, &USLGameplayAbility_Dodge::OnDodgeMontageEnded);
				AnimInst->Montage_SetEndDelegate(EndedDelegate, ActiveDodgeMontage);
				return; 
			}
		}
	}

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

UAnimMontage* USLGameplayAbility_Dodge::SelectDodgeMontage(const ACharacter* Character) const
{
    if (!Character) return DefaultDodgeMontage;

    const UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
    if (!MoveComp) return DefaultDodgeMontage;

    // State 1: Free Roaming (bOrientRotationToMovement == true)
    // Character automatically faces input direction, so always roll forward relative to themselves.
    if (MoveComp->bOrientRotationToMovement)
    {
        return DefaultDodgeMontage;
    }

    // State 2: Locked-On (bOrientRotationToMovement == false)
    // Read the current frame's directional input vector
    const FVector InputVector = Character->GetLastMovementInputVector();
    
    // If player presses dodge without moving the analog stick/WASD keys
    if (InputVector.IsNearlyZero())
    {
        return DodgeBackward;
    }

    // Transform input vector from World Space into Character Local Space
    const FVector LocalInput = Character->GetActorTransform().InverseTransformVectorNoScale(InputVector);
    
    // In Unreal: X is Forward, Y is Right. Atan2 gives us angles from -180 to +180 degrees.
    const float Angle = FMath::RadiansToDegrees(FMath::Atan2(LocalInput.Y, LocalInput.X));

    // Map the local angle into 45-degree wedges
    if (Angle >= -22.5f && Angle < 22.5f)    return DodgeForward;
    if (Angle >= 22.5f && Angle < 67.5f)     return DodgeForwardRight;
    if (Angle >= 67.5f && Angle < 112.5f)    return DodgeRight;
    if (Angle >= 112.5f && Angle < 157.5f)   return DodgeBackwardRight;
    if (Angle >= 157.5f || Angle < -157.5f)  return DodgeBackward;
    if (Angle >= -157.5f && Angle < -112.5f) return DodgeBackwardLeft;
    if (Angle >= -112.5f && Angle < -67.5f)  return DodgeLeft;
    if (Angle >= -67.5f && Angle < -22.5f)   return DodgeForwardLeft;

    return DefaultDodgeMontage;
}

void USLGameplayAbility_Dodge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bWasCancelled && ActiveDodgeMontage && ActorInfo)
	{
		if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (Character->GetMesh())
			{
				if (UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance())
				{
					AnimInst->Montage_Stop(0.15f, ActiveDodgeMontage);
				}
			}
		}
	}

	ActiveDodgeMontage = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USLGameplayAbility_Dodge::OnDodgeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActiveDodgeMontage) { return; }
    
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), /*bReplicateEnd*/ true, /*bWasCancelled*/ bInterrupted);
}