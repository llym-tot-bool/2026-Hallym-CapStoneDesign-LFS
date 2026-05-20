// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SLEnemyHPBarWidget.h"
#include "Components/ProgressBar.h"
#include "Blueprint/WidgetTree.h"

void USLEnemyHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!PB_HP && WidgetTree)
	{
		PB_HP = WidgetTree->FindWidget<UProgressBar>(TEXT("PB_HP"));
		if (!PB_HP)
		{
			PB_HP = WidgetTree->FindWidget<UProgressBar>(TEXT("ProgressBar"));
		}
	}
}

void USLEnemyHPBarWidget::SetHealthPercent(float InPercent)
{
	if (!PB_HP)
	{
		return;
	}

	PB_HP->SetPercent(FMath::Clamp(InPercent, 0.0f, 1.0f));
}
