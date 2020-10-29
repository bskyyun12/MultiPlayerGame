// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"

#include "ServerRow.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/PerceptiveAI/UI/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr))
	{
		return;
	}
	ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success)
	{
		return false;
	}

	if (!ensure(QuitButton != nullptr))
	{
		return false;
	}
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitButtonClicked);

	if (!ensure(HostButton != nullptr))
	{
		return false;
	}
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostButtonClicked);

	if (!ensure(JoinButton != nullptr))
	{
		return false;
	}
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinButtonClicked);

	if (!ensure(JoinGameButton != nullptr))
	{
		return false;
	}
	JoinGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinGameButtonClicked);

	if (!ensure(CancelButton != nullptr))
	{
		return false;
	}
	CancelButton->OnClicked.AddDynamic(this, &UMainMenu::OnCancelButtonClicked);

	if (!ensure(HostButton_HostMenu != nullptr))
	{
		return false;
	}
	HostButton_HostMenu->OnClicked.AddDynamic(this, &UMainMenu::OnHostButton_HostMenuClicked);

	return true;
}

void UMainMenu::OnQuitButtonClicked()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->QuitGame();
	}
}

void UMainMenu::OnHostButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Host Button Clicked!"));

	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::OnHostButton_HostMenuClicked()
{
	if (MenuInterface != nullptr)
	{
		if (GetServerNameTextBox() != nullptr)
		{
			const FString& Name = GetServerNameTextBox()->GetText().ToString();
			MenuInterface->Host(Name);
		}
		else
		{
			MenuInterface->Host("NoName");
		}
	}
}

void UMainMenu::OnJoinButtonClicked()
{
	if (!ensure(MenuSwitcher != nullptr))
	{
		return;
	}

	if (!ensure(JoinMenu != nullptr))
	{
		return;
	}

	MenuSwitcher->SetActiveWidget(JoinMenu);

	if (MenuInterface != nullptr)
	{
		MenuInterface->RefreshServerList();
	}
}

void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	ServerList->ClearChildren();

	uint32 i = 0;
	for (const FServerData& Server : ServerNames)
	{
		UServerRow* ServerRow = CreateWidget<UServerRow>(this, ServerRowClass);
		if (!ensure(ServerRow != nullptr))
		{
			return;
		}

		ServerRow->GetSessionId()->SetText(FText::FromString(Server.SessionID));
		ServerRow->GetHostUserName()->SetText(FText::FromString(Server.HostUserName));
		ServerRow->GetServerCapacityInfo()->SetText(FText::FromString(FString::Printf(TEXT("(%d/%d)"), Server.CurrentPlayers, Server.MaxPlayers)));

		ServerRow->Setup(this, i);
		++i;

		ServerList->AddChild(ServerRow);
	}
}

void UMainMenu::SetSelectIndex(uint32 Index)
{
	SelectedIndex = Index;
}

void UMainMenu::OnJoinGameButtonClicked()
{
	if (SelectedIndex.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index: %d"), SelectedIndex.GetValue());
		if (MenuInterface != nullptr)
		{
			MenuInterface->Join(SelectedIndex.GetValue());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index not set"));
	}

	//if (!ensure(IPAdress != nullptr))
	//{
	//	return;
	//}

	//const FString& Address = IPAdress->GetText().ToString();
	//if (MenuInterface != nullptr)
	//{
	//	MenuInterface->JoinWithIP(Address);
	//}
}

void UMainMenu::OnCancelButtonClicked()
{
	if (!ensure(MenuSwitcher != nullptr))
	{
		return;
	}

	if (!ensure(MainMenu != nullptr))
	{
		return;
	}

	MenuSwitcher->SetActiveWidget(MainMenu);
}
