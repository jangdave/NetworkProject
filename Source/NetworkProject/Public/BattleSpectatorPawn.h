// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "BattleSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API ABattleSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	class ANetworkProjectCharacter* originalPlayer;

	UFUNCTION()
	void RespawnPlayer();
};