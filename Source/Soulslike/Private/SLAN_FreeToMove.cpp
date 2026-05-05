


#include "SLAN_FreeToMove.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void USLAN_FreeToMove::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* owner = MeshComp->GetOwner();
	if (!owner) return;

	UAbilitySystemComponent* owner_asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(owner);
	if (!owner_asc) return;

	FGameplayEventData payload;
	payload.EventTag = EventTagStart;
	payload.Instigator = owner;
	owner_asc->HandleGameplayEvent(EventTagStart, &payload);

	//if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
	//{
	//	// Get the currently playing montage
	//	UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();

	//	if (CurrentMontage)
	//	{
	//		// Stop the montage. 
	//		// The float is the "Blend Out" time (e.g., 0.2s for a smooth transition)
	//		AnimInstance->Montage_Stop(0.2f, CurrentMontage);
	//	}
	//}
}
