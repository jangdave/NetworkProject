// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInfoWidget.h"
#include "../NetworkProjectCharacter.h"
#include "Components/ProgressBar.h"

void UPlayerInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	myPlayer = Cast<ANetworkProjectCharacter>(GetOwningPlayerPawn());

	if(myPlayer != nullptr)
	{
		SetHealth(myPlayer->GetHealth());
	}
}

void UPlayerInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(myPlayer == nullptr)
	{
		return;
	}
}

void UPlayerInfoWidget::SetPlayer(ANetworkProjectCharacter* player)
{
	player = myPlayer;
}

void UPlayerInfoWidget::SetHealth(const int32& value)
{
	if(myPlayer == nullptr)
	{
		return;
	}

	float calHP = (float)value / (float)myPlayer->maxHP;

	progressBar_HP->SetPercent(calHP);
}