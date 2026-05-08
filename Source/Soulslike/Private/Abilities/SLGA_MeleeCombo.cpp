


#include "Abilities/SLGA_MeleeCombo.h"

#include "SoulslikeCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void USLGA_MeleeCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!SLDA_WeaponCombo) {
        SimpleEndAbility(FString("no SLDA form activation")); return;
    }
    currentActionIdx = 0;
    lastActionIdx = SLDA_WeaponCombo->GA_list.Num() - 1;
    if (lastActionIdx < 0) {
        SimpleEndAbility(FString("GA_list is empty")); return;
    }

    tag_combo = SLDA_WeaponCombo->tag_combo;
    comboActionTagContainer.Reset();
    for (TSubclassOf<UGameplayAbility> GA : SLDA_WeaponCombo->GA_list) {
        const UGameplayAbility* defaultGA = GA->GetDefaultObject<UGameplayAbility>();
        if (!defaultGA) {
            SimpleEndAbility(FString("some GA from GA_list has no defaultGA")); return;
        }

        const FGameplayTagContainer tagCont_GA = defaultGA->GetAssetTags();
        if (tagCont_GA.Num() <= 0) {
            UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! melee combo ActivateAbility() : for some reason, there is no default tag for %s"),
                *GA->GetName());
            continue;
        }

        comboActionTagContainer.AddTag(tagCont_GA.First());
    }

    ASoulslikeCharacter* SLChar = Cast<ASoulslikeCharacter>(ActorInfo->AvatarActor);
    if (!SLChar) {
        EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
        return;
    }
    SLChar->delegate_CharacterMove.AddUObject(this, &USLGA_MeleeCombo::OnCharacterMove);

    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) {
        SimpleEndAbility(FString("no asc from ActiavteAbility()")); return;
    }
    
    StartAction();
}

void USLGA_MeleeCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

    if (TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo()) {
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USLGA_MeleeCombo::SimpleEndAbility(FString reason)
{
    UE_LOG(LogTemp, Display, TEXT("[SL debug] simple end : %s"), *reason);
    const FGameplayAbilitySpecHandle Handle = GetCurrentAbilitySpecHandle();
    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
    const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();

    // Call EndAbility with the retrieved data
    bool bReplicateEndAbility = true;
    EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, false);
}

void USLGA_MeleeCombo::StartAction()
{
    UE_LOG(LogTemp, Display, TEXT("[SL debug] start combo action"));
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();

    if (!SLDA_WeaponCombo) {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! %s StartAction() : no SLDA_WeaponCombo"),
            *this->GetName());
    }
    if (SLDA_WeaponCombo->GA_list.Num() <= 0) {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! %s StartAction() : GA_list is empty"),
            *this->GetName());
    }

    asc->TryActivateAbilityByClass(SLDA_WeaponCombo->GA_list[0]);
    FGameplayAbilitySpec* spec = asc->FindAbilitySpecFromClass(SLDA_WeaponCombo->GA_list[0]);
    currentComboAction = Cast<USLGA_MeleeSweep>(spec->GetPrimaryInstance());

    if (currentComboAction) {
        currentComboAction->delegate_ComboInput.AddUObject(this, &USLGA_MeleeCombo::OnComboInput);
        currentComboAction->delegate_Translation.AddUObject(this, &USLGA_MeleeCombo::OnTranslation);
        currentComboAction->delegate_Recovery.AddUObject(this, &USLGA_MeleeCombo::OnRecovery);
        state = ESL_MeleeSweep_State::Anticipation;
    }
    else {
        SimpleEndAbility(FString("StartAction() no current comboAction to observe")); return;
    }
}

void USLGA_MeleeCombo::PlayNextComboAction()
{
    currentComboAction->SimpleEndAbility();
    
    if (currentActionIdx == lastActionIdx) {
        SimpleEndAbility(FString("this is last combo action")); return;
    }
    currentActionIdx++;
    bIsInputBuffered = false;
    state = ESL_MeleeSweep_State::Anticipation;

    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();

    asc->TryActivateAbilityByClass(SLDA_WeaponCombo->GA_list[currentActionIdx]);
    FGameplayAbilitySpec* spec = asc->FindAbilitySpecFromClass(SLDA_WeaponCombo->GA_list[currentActionIdx]);
    currentComboAction = Cast<USLGA_MeleeSweep>(spec->GetPrimaryInstance());

    if (currentComboAction) {
        currentComboAction->delegate_ComboInput.AddUObject(this, &USLGA_MeleeCombo::OnComboInput);
        currentComboAction->delegate_Translation.AddUObject(this, &USLGA_MeleeCombo::OnTranslation);
        currentComboAction->delegate_Recovery.AddUObject(this, &USLGA_MeleeCombo::OnRecovery);
        state = ESL_MeleeSweep_State::Anticipation;
    }
    else {
        SimpleEndAbility(FString("failed to get combo action GA. so, failed to bind observation delegate")); return;
    }
}

void USLGA_MeleeCombo::OnComboInput()
{
    state = ESL_MeleeSweep_State::ComboInput;
}

void USLGA_MeleeCombo::OnTranslation()
{
    if (bIsInputBuffered) {
        PlayNextComboAction();
    }
    else {
        state = ESL_MeleeSweep_State::Translation;
    }
}

void USLGA_MeleeCombo::OnRecovery()
{
    SimpleEndAbility(FString("recovery"));
}

void USLGA_MeleeCombo::OnCharacterMove()
{
    SimpleEndAbility(FString("character move"));
}

void USLGA_MeleeCombo::OnPlayerInput(FGameplayTag tag)
{
    if (tag_combo != tag) return;

    switch (state) {
    case ESL_MeleeSweep_State::Anticipation:
        bIsInputBuffered = false;
        break;
    case ESL_MeleeSweep_State::ComboInput:
        bIsInputBuffered = true;
        break;
    case ESL_MeleeSweep_State::Translation:
        PlayNextComboAction();
        break;
    case ESL_MeleeSweep_State::Recovery:
        break;
    }
}

