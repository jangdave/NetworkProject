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
	// 세션을 생성한다

	// 온라인 세션 정보를 담은 IOnlineSubsystem 클래스를 가져온다
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
		// 세션 생성 정보를 만든다
		FOnlineSessionSettings sessionSettings;
		sessionSettings.bAllowInvites = false;
		sessionSettings.bAllowJoinInProgress = true;

		// 내가 가지고 있는 레벨을 가지고 있는 사람만 들어올수있음 / 업데이트의 유무
		sessionSettings.bAllowJoinViaPresence = true;

		// 데디케이트 서버
		sessionSettings.bIsDedicated = false;

		// 온라인 서브시스템의 이름이 "NULL" 이면 랜매칭하고 "Steam"이면 스팀서버로 매칭한다
		sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
		sessionSettings.NumPublicConnections = playerCount;
		sessionSettings.Set(FName("KEY_RoomName"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		// 남들이 검색을 할 수 있게 함, 공개방
		sessionSettings.bShouldAdvertise = true;

		// 세션을 생성한다
		sessionInterface->CreateSession(0, sessionID, sessionSettings);

		UE_LOG(LogTemp, Warning, TEXT("- Create Session try!"))
	}
}

// 만들어져 있는 세션을 찾는 함수
void UServerGameInstance::FindMySession()
{
	// 찾으려는 세션 쿼리를 생성한다
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	sessionSearch->MaxSearchResults = 30;

	// 특정 키워드로 검색필터기능 추가하고 싶을때 추가하는 쿼리 - UI 가 추가되어야 한다
	//sessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, FText("roomName"), EOnlineComparisonOp::GreaterThanEquals);

	// Presence로 생성된 세션을 필터링하고 싶은때
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// 앞에서 만든 쿼리를 이용해서 세션을 찾는다
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

// 찾은 세션 리스트 중에서 특정 세션에 들어가고자 할때 사용할 함수
void UServerGameInstance::JoinMySession(int32 sessionIdx)
{
	// 인덱스로 세션을 선택하고 그 세션으로 조인한다
	FOnlineSessionSearchResult selectedSession = sessionSearch->SearchResults[sessionIdx];

	sessionInterface->JoinSession(0, sessionID, selectedSession);
}

// 세션이 서버에 만들어졌을때 호출된 함수
void UServerGameInstance::OnCreateSessionComplete(FName sessionName, bool bIsSuccess)
{
	FString result = bIsSuccess ? TEXT("Create Session Success!") : TEXT("Create Session Failed...");

	UE_LOG(LogTemp, Warning, TEXT("%s : %s"), *result, *sessionName.ToString());

	// 세션 생성에 성공했다면, 전투 맵쪽으로 세션에 접속한 모든 인원을 이동시킨다
	if(bIsSuccess)
	{
		GetWorld()->ServerTravel("/Game/Maps/MainMap?Listen");
	}
}

// 세션 검색이 끝났을때 호출될 함수
void UServerGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// 검색 결과를 배열에 담고 몇개나 담겼는지 로그 확인
		TArray<FOnlineSessionSearchResult> searchResults = sessionSearch->SearchResults;
		UE_LOG(LogTemp, Warning, TEXT("Find Session Count : %d"), searchResults.Num());

		// 검색 결과를 모두 확인해 본다
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

			// 슬롯에 필요한 정보를 이벤트로 호출한다
			//searchResultDele.Broadcast(roomName, currentPlayers, maxPlayers, ping, i);
			// 덩어리로 넣을수 있다
			searchResultDele.Broadcast(searchedSessionInfo);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Sessions Failed..."));
	}

	// 새로고침 버튼을 다시 활성화 한다
	searchFinishedDele.Broadcast();
}

// 다른 세션에 합류 처리가 끝났을때 호출되는 이벤트 함수
void UServerGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type joinResult)
{
	// 만일 join에 성공했다면 해당 IP Address 레벨로 이동을 한다
	if(joinResult == EOnJoinSessionCompleteResult::Success)
	{
		// 세션 이름으로 IP주소를 획득한다
		FString joinAddress;

		sessionInterface->GetResolvedConnectString(sessionName, joinAddress);

		UE_LOG(LogTemp, Warning, TEXT("join address : %s"), *joinAddress)

		if(APlayerController* pc = GetWorld()->GetFirstPlayerController())
		{
			pc->ClientTravel(joinAddress, ETravelType::TRAVEL_Absolute);
		}

	}
}
