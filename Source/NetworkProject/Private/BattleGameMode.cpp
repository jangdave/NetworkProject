// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

/*AActor* ABattleGameMode::ChoosePlayerStart(AController* player)
{
	// ���� �ȿ� �ִ� �÷��̾� ��ŸƮ ���͵��� �����´�
	for(TActorIterator<APlayerStart> iter(GetWorld()); iter; ++iter)
	{
		APlayerStart* ps = *iter;

		if(ps->PlayerStartTag != FName("Spawned"))
		{
			// ��ȯ�� �÷��̾� ��ŸƮ ������ �±׸� "spawned" �߰��Ѵ�
			ps->PlayerStartTag = FName("Spawned");
			return ps;
		}
	}

	return nullptr;
}*/

AActor* ABattleGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	for (TActorIterator<APlayerStart> iter(GetWorld()); iter; ++iter)
	{
		APlayerStart* ps = *iter;

		if (ps->PlayerStartTag != FName("Spawned"))
		{
			// ��ȯ�� �÷��̾� ��ŸƮ ������ �±׸� "spawned" �߰��Ѵ�
			ps->PlayerStartTag = FName("Spawned");
			return ps;
		}
	}

	return nullptr;
}
