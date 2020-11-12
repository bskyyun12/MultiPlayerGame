// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "MultiplayerGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++CurrentPlayersLoggedIn;
	UE_LOG(LogTemp, Warning, TEXT("CurrentPlayersLoggedIn: %d"), CurrentPlayersLoggedIn);

	if (CurrentPlayersLoggedIn >= 2)
	{
		UMultiplayerGameInstance* GameInstance = Cast<UMultiplayerGameInstance>(GetGameInstance());
		if (GameInstance == nullptr)
		{
			return;
		}
		GameInstance->StartSession();

		UWorld* World = GetWorld();
		if (!ensure(World != nullptr))
		{
			return;
		}
		FTimerHandle GameStartTimer;
		World->GetTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame, StartDelay);
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	--CurrentPlayersLoggedIn;

	UE_LOG(LogTemp, Warning, TEXT("CurrentPlayersLoggedIn: %d"), CurrentPlayersLoggedIn);
}

void ALobbyGameMode::StartGame()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
	{
		return;
	}

	bUseSeamlessTravel = true;
	World->ServerTravel(MAINGAME_URL);
}
