// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayGameGameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYGAME_API ALobbyGameMode : public AMultiplayGameGameModeBase
{
	GENERATED_BODY()
	
public:
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;

private:
	void StartGame();

private:
	uint16 StartDelay = 10;
	uint16 NumOfPlayersRequiredToStartGame = 3;
	uint16 CurrentPlayersLoggedIn = 0;

	const FString& MAINGAME_URL = TEXT("/Game/Maps/MultiplayMap?listen");
};
