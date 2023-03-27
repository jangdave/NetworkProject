// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "ServerGameInstance.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/WidgetSwitcher.h"
#include "SessionSlotWidget.h"
#include "Components/ScrollBox.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Ȥ�� �� �Է� �ʵ��� ���� �� ĭ���� �ʱ�ȭ�Ѵ�
	editText_id->SetText(FText::FromString(""));
	editText_RoomName->SetText(FText::FromString(""));
	sl_PlayerCount->SetValue(2.0f);
	text_PlayerCount->SetText(FText::FromString("2"));

	// ��ư Ŭ�� �̺�Ʈ�� �Լ� �����ϱ�
	btn_Start->OnClicked.AddDynamic(this, &ULoginWidget::ClickStart);
	btn_CreateSession->OnClicked.AddDynamic(this, &ULoginWidget::CreateServer);
	sl_PlayerCount->OnValueChanged.AddDynamic(this, &ULoginWidget::OnMoveSlide);
	btn_GoCreate->OnClicked.AddDynamic(this, &ULoginWidget::GoCreate);
	btn_GoFind->OnClicked.AddDynamic(this, &ULoginWidget::GoFind);
	btn_CreateBack->OnClicked.AddDynamic(this, &ULoginWidget::GoBack);
	btn_FindBack->OnClicked.AddDynamic(this, &ULoginWidget::GoBack);
	btn_Refresh->OnClicked.AddDynamic(this, &ULoginWidget::RefreshList);

	gameInstance = Cast<UServerGameInstance>(GetGameInstance());

	if(gameInstance != nullptr)
	{
		gameInstance->searchResultDele.AddDynamic(this, &ULoginWidget::AddNewSlot);
		gameInstance->searchFinishedDele.AddDynamic(this, &ULoginWidget::RefreshEnabled);
	}
}

void ULoginWidget::ClickStart()
{
	// ���� id�� ��ĭ�� �ƴ϶�� 0�� ĵ�������� 1�� ĵ������ ����
	if(!editText_id->GetText().IsEmpty())
	{
		widgetSwitcher->SetActiveWidgetIndex(1);

		gameInstance->sessionID = FName(*editText_id->GetText().ToString());
	}
}

void ULoginWidget::CreateServer()
{
	int32 playerCnt = FMath::RoundFromZero(sl_PlayerCount->GetValue());

	gameInstance->CreateMySession(editText_RoomName->GetText().ToString(), playerCnt);
}

// �����̴� ���� ����ɶ� ȣ��Ǵ� �Լ�
void ULoginWidget::OnMoveSlide(float value)
{
	//FString num2str = FString::Printf(TEXT("%d"), FMath::RoundToInt32(value));
	int32 newVal = FMath::RoundHalfFromZero(value);
	text_PlayerCount->SetText(FText::AsNumber(newVal));
}

void ULoginWidget::GoCreate()
{
	widgetSwitcher->SetActiveWidgetIndex(2);
}

void ULoginWidget::GoFind()
{
	widgetSwitcher->SetActiveWidgetIndex(3);

	RefreshList();
}

void ULoginWidget::GoBack()
{
	widgetSwitcher->SetActiveWidgetIndex(1);
}

void ULoginWidget::RefreshList()
{
	// �ʱ�ȭ
	sbox_RoomList->ClearChildren();

	gameInstance->FindMySession();

	btn_Refresh->SetIsEnabled(false);
}

// ���� �ν��Ͻ��κ��� �˻� �Ϸ� �̺�Ʈ�� �޾����� ����� �Լ�
void ULoginWidget::AddNewSlot(FSessionInfo sessionInfo)
{
	USessionSlotWidget* slotWidget = CreateWidget<USessionSlotWidget>(this, sessionSlot);

	if(slotWidget != nullptr)
	{
		slotWidget->text_roomName->SetText(FText::FromString(sessionInfo.roomName));
		slotWidget->text_playerInfo->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), sessionInfo.currentPlayers, sessionInfo.maxPlayers)));
		slotWidget->text_ping->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), sessionInfo.ping)));
		slotWidget->index = sessionInfo.idx;

		// ��ũ�ѹڽ��� �־��ش�
		sbox_RoomList->AddChild(slotWidget);
	}
}

void ULoginWidget::RefreshEnabled()
{
	btn_Refresh->SetIsEnabled(true);
}

