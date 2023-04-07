// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerController.h"
#include "BattleGameMode.h"
#include "MainWidget.h"
#include "GameFramework/GameModeBase.h"
#include "../NetworkProjectCharacter.h"

void ABattlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(mainWidget != nullptr && IsLocalController())
	{
		UMainWidget* mainUI = CreateWidget<UMainWidget>(this, mainWidget);

		if(mainUI)
		{
			mainUI->AddToViewport();
		}
	}
}

void ABattlePlayerController::Respawn(class ANetworkProjectCharacter* player)
{
	if(HasAuthority())
	{
		ABattleGameMode* gm = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());

		if(gm != nullptr)
		{
			player->Destroy();

			gm->RestartPlayer(this);
		}
	}
}
