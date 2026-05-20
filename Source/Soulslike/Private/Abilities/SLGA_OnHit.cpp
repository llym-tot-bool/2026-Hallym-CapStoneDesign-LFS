


#include "Abilities/SLGA_OnHit.h"
#include "SoulslikeCharacter.h"
#include "SoulslikePlayerState.h"
#include "Soulslike.h"


void USLGA_OnHit::setRootMotion()
{
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) return;

    if (!asc->HasMatchingGameplayTag(tag_RootMotion)) {
        asc->AddLooseGameplayTag(tag_RootMotion);
    }

    if (asc->HasMatchingGameplayTag(tag_IsMoving)) {
        asc->RemoveLooseGameplayTag(tag_IsMoving);
    }
}

void USLGA_OnHit::removeRootMotion()
{
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) return;

    if (asc->HasMatchingGameplayTag(tag_RootMotion)) {
        asc->RemoveLooseGameplayTag(tag_RootMotion);
    }
}

bool USLGA_OnHit::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return true;
}

void USLGA_OnHit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    state = ESL_Melee_State::Anticipation;
    setRootMotion();

    // delegate binding
    ASC = GetAbilitySystemComponentFromActorInfo(); ensureOrQuit(ASC);
    SLPS = Cast<ASoulslikePlayerState>(ASC->GetOwner()); ensureOrQuit(SLPS);

    ASoulslikeCharacter* SLChar = Cast<ASoulslikeCharacter>(ActorInfo->AvatarActor); ensureOrQuit(SLChar);
    SLChar->delegate_CharacterMove.AddUObject(this, &USLGA_OnHit::OnCharacteMove);
}

void USLGA_OnHit::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    // remove delegate biindings
    if (SLPS) {
        SLPS->delegate_MeleeSweep_State.RemoveAll(this);
    }

    ASoulslikeCharacter* SLChar = Cast<ASoulslikeCharacter>(ActorInfo->AvatarActor);
    if (SLChar) {
        SLChar->delegate_CharacterMove.RemoveAll(this);
    }

    switch (state) {
        case ESL_Melee_State::Anticipation:
            SLDEBUG("non combo End ability with sweep state : Anticipation")
                Recovery();
            break;
        case ESL_Melee_State::Recovery:
            break;
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USLGA_OnHit::ChangeState(ESL_Melee_State newstate)
{
    switch (newstate)
    {
    case ESL_Melee_State::Recovery:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate arrived : Recovery "));
        Recovery();
        break;
    default:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! melee sweep wrong tag"));
        break;
    }
}

void USLGA_OnHit::OnCharacteMove()
{
    UE_LOG(LogTemp, Display, TEXT("[SL debug] melee canceld with character move"));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void USLGA_OnHit::Recovery()
{
    state = ESL_Melee_State::Recovery;
    SLDEBUG("Recovery boradcasted from = %s", *this->GetName());
    removeRootMotion();
    delegate_Recovery.Broadcast();
}
