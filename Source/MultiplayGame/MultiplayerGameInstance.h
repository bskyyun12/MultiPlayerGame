// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MenuSystem/MenuInterface.h"
#include "MultiplayerGameInstance.generated.h"

/**
 *
 */
UCLASS()
class MULTIPLAYGAME_API UMultiplayerGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:
	UMultiplayerGameInstance(const FObjectInitializer& ObjectIn);

	virtual void Init();

	UFUNCTION(Exec, BlueprintCallable)
	void LoadInGameMenu();

	UFUNCTION(Exec, BlueprintCallable)
	void LoadMainMenu(bool bLoadMainMenuLevel) override;

	UFUNCTION(Exec)
	void QuitGame() override;

	UFUNCTION(Exec)
	void Host(const FString& ServerName) override;

	UFUNCTION(Exec)
	void RefreshServerList() override;

	UFUNCTION(Exec)
	void JoinWithIP(const FString& Address) override;

	UFUNCTION(Exec)
	void Join(uint32 Index) override;

private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void CreateSession(const FString& ServerName);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	TSubclassOf<class UUserWidget> MenuClass;
	class UMainMenu* Menu;

	TSubclassOf<class UUserWidget> InGameMenuClass;

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
};
