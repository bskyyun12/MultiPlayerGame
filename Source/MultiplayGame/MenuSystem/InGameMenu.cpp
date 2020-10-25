// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success)
	{
		return false;
	}

	if (!ensure(CancelButton != nullptr))
	{
		return false;
	}
	CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::OnCancelButtonClicked);

	if (!ensure(QuitButton != nullptr))
	{
		return false;
	}
	QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::OnQuitButtonClicked);

	return true;
}

void UInGameMenu::OnCancelButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnCancelButtonClicked"));
	Teardown();
}

void UInGameMenu::OnQuitButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnQuitButtonClicked"));
	if (MenuInterface != nullptr)
	{
		Teardown();
		MenuInterface->LoadMainMenu(true);
	}
}