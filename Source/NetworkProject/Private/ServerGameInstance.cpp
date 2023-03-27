// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UServerGameInstance::UServerGameInstance()
{
	sessionID = "Test Session";
}

void UServerGameInstance::Init()
{
	Super::Init();
	// ������ �����Ѵ�

	// �¶��� ���� ������ ���� IOnlineSubsystem Ŭ������ �����´�
	IOnlineSubsystem* subsys = IOnlineSubsystem::Get();

	if(subsys)
	{
		sessionInterface = subsys->GetSessionInterface();

		if(sessionInterface != nullptr)
		{
			sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UServerGameInstance::OnCreateSessionComplete);
			sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UServerGameInstance::OnFindSessionComplete);
			sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UServerGameInstance::OnJoinSessionComplete);
		}

		FString platformName = subsys->GetSubsystemName().ToString();
		UE_LOG(LogTemp, Warning, TEXT("platform name : %s"), *platformName);
	}
}

void UServerGameInstance::CreateMySession(FString roomName, int32 playerCount)
{
	if(sessionInterface != nullptr)
	{
		// ���� ���� ������ �����
		FOnlineSessionSettings sessionSettings;
		sessionSettings.bAllowInvites = false;
		sessionSettings.bAllowJoinInProgress = true;

		// ���� ������ �ִ� ������ ������ �ִ� ����� ���ü����� / ������Ʈ�� ����
		sessionSettings.bAllowJoinViaPresence = true;

		// ��������Ʈ ����
		sessionSettings.bIsDedicated = false;

		// �¶��� ����ý����� �̸��� "NULL" �̸� ����Ī�ϰ� "Steam"�̸� ���������� ��Ī�Ѵ�
		sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
		sessionSettings.NumPublicConnections = playerCount;
		sessionSettings.Set(FName("KEY_RoomName"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		// ������ �˻��� �� �� �ְ� ��, ������
		sessionSettings.bShouldAdvertise = true;

		// ������ �����Ѵ�
		sessionInterface->CreateSession(0, sessionID, sessionSettings);

		UE_LOG(LogTemp, Warning, TEXT("- Create Session try!"))
	}
}

// ������� �ִ� ������ ã�� �Լ�
void UServerGameInstance::FindMySession()
{
	// ã������ ���� ������ �����Ѵ�
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	sessionSearch->MaxSearchResults = 30;

	// Ư�� Ű����� �˻����ͱ�� �߰��ϰ� ������ �߰��ϴ� ���� - UI �� �߰��Ǿ�� �Ѵ�
	//sessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, FText("roomName"), EOnlineComparisonOp::GreaterThanEquals);

	// Presence�� ������ ������ ���͸��ϰ� ������
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// �տ��� ���� ������ �̿��ؼ� ������ ã�´�
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

// ã�� ���� ����Ʈ �߿��� Ư�� ���ǿ� ������ �Ҷ� ����� �Լ�
void UServerGameInstance::JoinMySession(int32 sessionIdx)
{
	// �ε����� ������ �����ϰ� �� �������� �����Ѵ�
	FOnlineSessionSearchResult selectedSession = sessionSearch->SearchResults[sessionIdx];

	sessionInterface->JoinSession(0, sessionID, selectedSession);
}

// ������ ������ ����������� ȣ��� �Լ�
void UServerGameInstance::OnCreateSessionComplete(FName sessionName, bool bIsSuccess)
{
	FString result = bIsSuccess ? TEXT("Create Session Success!") : TEXT("Create Session Failed...");

	UE_LOG(LogTemp, Warning, TEXT("%s : %s"), *result, *sessionName.ToString());

	// ���� ������ �����ߴٸ�, ���� �������� ���ǿ� ������ ��� �ο��� �̵���Ų��
	if(bIsSuccess)
	{
		GetWorld()->ServerTravel("/Game/Maps/MainMap?Listen");
	}
}

// ���� �˻��� �������� ȣ��� �Լ�
void UServerGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// �˻� ����� �迭�� ��� ��� ������ �α� Ȯ��
		TArray<FOnlineSessionSearchResult> searchResults = sessionSearch->SearchResults;
		UE_LOG(LogTemp, Warning, TEXT("Find Session Count : %d"), searchResults.Num());

		// �˻� ����� ��� Ȯ���� ����
		for(int32 i = 0; i<searchResults.Num(); i++)
		{
			FSessionInfo searchedSessionInfo;

			FString roomName;
			searchResults[i].Session.SessionSettings.Get(FName("KEY_RoomName"), roomName);
			searchedSessionInfo.roomName = roomName;

			int32 maxPlayers = searchResults[i].Session.SessionSettings.NumPublicConnections;
			searchedSessionInfo.maxPlayers = maxPlayers;

			int32 currentPlayers = maxPlayers - searchResults[i].Session.NumOpenPublicConnections;
			searchedSessionInfo.currentPlayers = currentPlayers;

			int32 ping = searchResults[i].PingInMs;
			searchedSessionInfo.ping = ping;

			searchedSessionInfo.idx = i;

			// ���Կ� �ʿ��� ������ �̺�Ʈ�� ȣ���Ѵ�
			//searchResultDele.Broadcast(roomName, currentPlayers, maxPlayers, ping, i);
			// ����� ������ �ִ�
			searchResultDele.Broadcast(searchedSessionInfo);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Sessions Failed..."));
	}

	// ���ΰ�ħ ��ư�� �ٽ� Ȱ��ȭ �Ѵ�
	searchFinishedDele.Broadcast();
}

// �ٸ� ���ǿ� �շ� ó���� �������� ȣ��Ǵ� �̺�Ʈ �Լ�
void UServerGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type joinResult)
{
	// ���� join�� �����ߴٸ� �ش� IP Address ������ �̵��� �Ѵ�
	if(joinResult == EOnJoinSessionCompleteResult::Success)
	{
		// ���� �̸����� IP�ּҸ� ȹ���Ѵ�
		FString joinAddress;

		sessionInterface->GetResolvedConnectString(sessionName, joinAddress);

		UE_LOG(LogTemp, Warning, TEXT("join address : %s"), *joinAddress)

		if(APlayerController* pc = GetWorld()->GetFirstPlayerController())
		{
			pc->ClientTravel(joinAddress, ETravelType::TRAVEL_Absolute);
		}

	}
}
