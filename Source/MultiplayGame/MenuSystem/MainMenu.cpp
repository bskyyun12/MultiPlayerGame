// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
// Test
#include <TimerManager.h>

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
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);

	if (!ensure(HostMenuButton != nullptr))
	{
		return false;
	}
	HostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

	if (!ensure(JoinMenuButton != nullptr))
	{
		return false;
	}
	JoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(JoinServerButton != nullptr))
	{
		return false;
	}
	JoinServerButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (!ensure(BackButtonInJoinMenu != nullptr))
	{
		return false;
	}
	BackButtonInJoinMenu->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(HostServerButton != nullptr))
	{
		return false;
	}
	HostServerButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
	
	if (!ensure(BackButtonInHostMenu != nullptr))
	{
		return false;
	}
	BackButtonInHostMenu->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	return true;
}

void UMainMenu::QuitGame()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->QuitGame();
	}
}

void UMainMenu::OpenHostMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Host Button Clicked!"));

	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::HostServer()
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

void UMainMenu::OpenJoinMenu()
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

void UMainMenu::JoinServer()
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

void UMainMenu::OpenMainMenu()
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
