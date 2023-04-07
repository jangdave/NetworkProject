// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSpectatorPawn.h"
#include "BattlePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NetworkProject/NetworkProjectCharacter.h"

void ABattleSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle spawnTimer;
	GetWorldTimerManager().SetTimer(spawnTimer, FTimerDelegate::CreateLambda([&]()
	{
		RespawnPlayer();

	}), 5.0f, false);
}

void ABattleSpectatorPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABattleSpectatorPawn::RespawnPlayer()
{
	if(HasAuthority())
	{
		ABattlePlayerController* pc = Cast<ABattlePlayerController>(GetController());

		if(pc != nullptr && originalPlayer != nullptr)
		{
			pc->Possess(originalPlayer);
			pc->Respawn(originalPlayer);
		}
	}
}
