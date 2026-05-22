

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLAN_PlaySound.generated.h"


UCLASS()
class SOULSLIKE_API USLAN_PlaySound : public UAnimNotify
{
	GENERATED_BODY()

public:	
    UPROPERTY(EditAnywhere, Category = "SoundLocation")
    FName socket_for_sound;
    UPROPERTY(EditAnywhere, Category = "Sound")
    TObjectPtr<USoundBase> Sound;

    virtual void Notify(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;
	
};
