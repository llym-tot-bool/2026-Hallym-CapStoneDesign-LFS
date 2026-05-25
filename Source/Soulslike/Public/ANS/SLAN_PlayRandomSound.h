

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "SLAN_PlayRandomSound.generated.h"


UCLASS()
class SOULSLIKE_API USLAN_PlayRandomSound : public UAnimNotify
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
