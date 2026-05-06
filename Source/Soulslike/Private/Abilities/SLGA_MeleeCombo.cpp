


#include "Abilities/SLGA_MeleeCombo.h"

#include "SoulslikeCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void USLGA_MeleeCombo::addTag(TObjectPtr<UAbilitySystemComponent> asc, FGameplayTag tag)
{
    if (!asc->HasMatchingGameplayTag(tag)) asc->AddLooseGameplayTag(tag);
}

void USLGA_MeleeCombo::removeTag(TObjectPtr<UAbilitySystemComponent> asc, FGameplayTag tag)
{
    if (asc->HasMatchingGameplayTag(tag)) asc->RemoveLooseGameplayTag(tag);
}

void USLGA_MeleeCombo::StartComboChcker()
{
    if (comboChecker) {
        comboChecker->EndTask();
        comboChecker = USLAT_MeeleComboChecker::Create(this, tag_comboAvailable, tag_comboGrant, tag_comboPerform);
    }
}

void USLGA_MeleeCombo::EndComboChecker()
{
    if (comboChecker) {
        comboChecker->EndTask();
        comboChecker = nullptr;
    }
}

void USLGA_MeleeCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!SLDA_WeaponCombo) return;
    currentActionIdx = 0;
    lastActionIdx = SLDA_WeaponCombo->GA_list.Num() - 1;
    if (lastActionIdx < 0) return;

    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) return;

    if (UAbilityTask_WaitGameplayEvent* waitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_inputAsComboStart)) {
        waitEvent->EventReceived.AddDynamic(this, &USLGA_MeleeCombo::InputAsComboStart);
        waitEvent->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : tag_inputAsComboStart listener failed"), *this->GetName());
    }

    if (UAbilityTask_WaitGameplayEvent* waitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_inputAsComboEnd)) {
        waitEvent->EventReceived.AddDynamic(this, &USLGA_MeleeCombo::InputAsComboEnd);
        waitEvent->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : tag_inputAsComboEnd listener failed"), *this->GetName());
    }

    if (UAbilityTask_WaitGameplayEvent* waitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_comboAvailable)) {
        waitEvent->EventReceived.AddDynamic(this, &USLGA_MeleeCombo::ComboAvailable);
        waitEvent->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : tag_comboAvailable listener failed"), *this->GetName());
    }

    
    if (UAbilityTask_WaitGameplayEvent* waitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_comboPerform)) {
        waitEvent->EventReceived.AddDynamic(this, &USLGA_MeleeCombo::ComboPerform);
        waitEvent->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : tag_comboPerform listener failed"), *this->GetName());
    }
    
    StartAction();
}

void USLGA_MeleeCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    EndComboChecker();

    if (TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo()) {
        removeTag(asc, tag_inputAsComboStart);
        removeTag(asc, tag_inputAsComboEnd);
        removeTag(asc, tag_comboAvailable);
        removeTag(asc, tag_comboGrant);
        removeTag(asc, tag_comboLastAction);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USLGA_MeleeCombo::StartAction()
{
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
}

void USLGA_MeleeCombo::InputAsComboStart(FGameplayEventData Payload)
{
    if (TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo()) {
        addTag(asc, tag_inputAsComboStart);
    }
}

void USLGA_MeleeCombo::InputAsComboEnd(FGameplayEventData Payload)
{
    EndComboChecker();

    if (TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo()) {
        removeTag(asc, tag_inputAsComboStart);
        removeTag(asc, tag_comboAvailable);
        addTag(asc, tag_inputAsComboEnd);
    }
}

void USLGA_MeleeCombo::ComboAvailable(FGameplayEventData Payload)
{
    if (TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo()) {
        addTag(asc, tag_comboAvailable);

        StartComboChcker();
    }
}

void USLGA_MeleeCombo::ComboPerform(FGameplayEventData Payload)
{
    EndComboChecker();

    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) return;
    if (!SLDA_WeaponCombo) return;
    if (currentActionIdx >= lastActionIdx) return;
    
    currentActionIdx++;
    asc->TryActivateAbilityByClass(SLDA_WeaponCombo->GA_list[currentActionIdx]);
    
    if (currentActionIdx == lastActionIdx) {
        addTag(asc, tag_comboLastAction);
    }
}

USLAT_MeeleComboChecker* USLAT_MeeleComboChecker::Create(UGameplayAbility* OwningAbility, 
    const FGameplayTag tag_comboAvailable, const FGameplayTag tag_comboGrant, const FGameplayTag tag_comboPerform)
{
    USLAT_MeeleComboChecker* task = NewAbilityTask<USLAT_MeeleComboChecker>(OwningAbility);
    task->tag_comboAvailable = tag_comboAvailable;
    task->tag_comboGrant = tag_comboGrant;
    task->tag_comboPerform = tag_comboPerform;

    task->bTickingTask = true;
    return task;
}

void USLAT_MeeleComboChecker::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);
    TObjectPtr<UAbilitySystemComponent> asc = AbilitySystemComponent.Get();
    if (!asc) return;

    if (asc->HasMatchingGameplayTag(tag_comboAvailable) && asc->HasMatchingGameplayTag(tag_comboGrant)) {
        FGameplayEventData payload;
        payload.EventTag = tag_comboPerform;
        asc->HandleGameplayEvent(tag_comboPerform, &payload);
    }
}
