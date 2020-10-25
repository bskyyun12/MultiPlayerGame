// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

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
	if (MenuInterface != nullptr)
	{
		MenuInterface->Host();
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
}

void UMainMenu::OnJoinGameButtonClicked()
{
	if (!ensure(IPAdress != nullptr))
	{
		return;
	}

	const FString& Address = IPAdress->GetText().ToString();
	if (MenuInterface != nullptr)
	{
		MenuInterface->Join(Address);
	}
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
