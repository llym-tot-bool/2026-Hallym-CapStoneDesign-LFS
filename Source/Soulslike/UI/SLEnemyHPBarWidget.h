// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLEnemyHPBarWidget.generated.h"

class SProgressBar;

UCLASS()
class SOULSLIKE_API USLEnemyHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UFUNCTION(BlueprintCallable, Category="Enemy|UI")
	void SetHealthPercent(float InPercent);

private:
	float CachedHealthPercent = 1.0f;
	TSharedPtr<SProgressBar> SlateHealthBar;
};
