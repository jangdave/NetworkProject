// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "../NetworkProjectCharacter.h"
#include "Components/TextBlock.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 컨트롤러를 캐릭터로 캐스팅
	player = Cast<ANetworkProjectCharacter>(GetOwningPlayerPawn());

}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	text_Health->SetText(FText::AsNumber(player->GetHealth()));
	text_Ammo->SetText(FText::AsNumber(player->GetAmmo()));
}
