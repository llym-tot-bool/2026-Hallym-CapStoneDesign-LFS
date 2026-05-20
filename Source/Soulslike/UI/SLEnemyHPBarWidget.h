// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLEnemyHPBarWidget.generated.h"

class UProgressBar;

UCLASS()
class SOULSLIKE_API USLEnemyHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category="Enemy|UI")
	void SetHealthPercent(float InPercent);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> PB_HP;
};
