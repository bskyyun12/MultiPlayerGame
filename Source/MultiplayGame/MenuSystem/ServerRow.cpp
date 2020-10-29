// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "MainMenu.h"

void UServerRow::Setup(class UMainMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;
	ServerButton->OnClicked.AddDynamic(this, &UServerRow::OnButtonClicked);
}

void UServerRow::OnButtonClicked()
{
	Parent->SetSelectIndex(Index);

	UE_LOG(LogTemp, Warning, TEXT("Clicked: %s"), *SessionId->GetText().ToString());

	UPanelWidget* ServerList = Parent->GetServerList();
	for (int32 i = 0; i < ServerList->GetChildrenCount(); i++)
	{
		UServerRow* ServerRow = Cast<UServerRow>(ServerList->GetChildAt(i));
		ServerRow->bisSelected = (Index == i);
	}
}
