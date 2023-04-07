// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "BattleGameStateBase.h"
#include "../NetworkProjectCharacter.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 컨트롤러를 캐릭터로 캐스팅
	player = Cast<ANetworkProjectCharacter>(GetOwningPlayerPawn());

	btn_Quit->OnClicked.AddDynamic(this, &UMainWidget::QuitSession);
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	text_Health->SetText(FText::AsNumber(player->GetHealth()));
	text_Ammo->SetText(FText::AsNumber(player->GetAmmo()));

	FString playerList;

	ABattleGameStateBase* gs = Cast<ABattleGameStateBase>(GetWorld()->GetGameState());

	for(const auto& ps : gs->GetPlayerListByScore())
	{
		FString playerName = ps->GetPlayerName();

		int32 playerScore = ps->GetScore();

		playerList.Append(FString::Printf(TEXT("%s : %d\n"), *playerName, playerScore));
	}

	text_PlayerList->SetText(FText::FromString(playerList));
}

void UMainWidget::QuitSession()
{
	if(player != nullptr)
	{
		player->EndSession();
	}
}
