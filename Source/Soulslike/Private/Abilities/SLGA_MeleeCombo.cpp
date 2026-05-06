


#include "Abilities/SLGA_MeleeCombo.h"

#include "SoulslikeCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void USLGA_MeleeCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) return;

    if (UAbilityTask_WaitGameplayEvent* waitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_inputAsComboStart)) {
        waitEvent->EventReceived.AddDynamic(this, &USLGA_MeleeCombo::ComboPerform);
        waitEvent->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : tag_inputAsComboStart listener failed"), *this->GetName());
    }

    if (UAbilityTask_WaitGameplayEvent* waitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_inputAsComboEnd)) {
        waitEvent->EventReceived.AddDynamic(this, &USLGA_MeleeCombo::ComboPerform);
        waitEvent->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : tag_inputAsComboEnd listener failed"), *this->GetName());
    }
    
    if (UAbilityTask_WaitGameplayEvent* waitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_comboPerform)) {
        waitEvent->EventReceived.AddDynamic(this, &USLGA_MeleeCombo::ComboPerform);
        waitEvent->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : tag_comboPerform listener failed"), *this->GetName());
    }
    
}

void USLGA_MeleeCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USLGA_MeleeCombo::InputAsComboStart(FGameplayEventData Payload)
{
    if (comboChecker) {
        comboChecker->EndTask();
    }
    comboChecker = USLAT_MeeleComboChecker::Create(this, tag_comboAvailable, tag_comboGrant, tag_comboPerform);
    comboChecker->ReadyForActivation();
}

void USLGA_MeleeCombo::InputAsComboEnd(FGameplayEventData Payload)
{
    comboChecker->EndTask();
}

void USLGA_MeleeCombo::ComboPerform(FGameplayEventData Payload)
{
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) return;

    //asc->TryActivateAbilityByClass(comboActoin_list[currentActionIdx]);
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
