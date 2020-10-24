// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameInstance.h"

#include "Engine/Engine.h"

UMultiplayerGameInstance::UMultiplayerGameInstance(const FObjectInitializer& ObjectIn)
{
	UE_LOG(LogTemp, Warning, TEXT("UMultiplayerGameInstance Ctr"));
}

void UMultiplayerGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("UMultiplayerGameInstance Init"));
}

void UMultiplayerGameInstance::Host()
{
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr))
	{
		return;
	}
	Engine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
	{
		return;
	}
	World->ServerTravel("/Game/Maps/MultiplayMap?listen");
}

void UMultiplayerGameInstance::Join(const FString& Address)
{
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr))
	{
		return;
	}	
	Engine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Joinning: %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController !=nullptr))
	{
		return;
	}
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}
