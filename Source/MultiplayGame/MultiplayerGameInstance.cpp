// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameInstance.h"

#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"

#include "MenuSystem/MainMenu.h"
#include "MenuSystem/InGameMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName SESSION_KEY = TEXT("My Session Key");

UMultiplayerGameInstance::UMultiplayerGameInstance(const FObjectInitializer& ObjectIn)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/PerceptiveAI/UI/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr))
	{
		return;
	}
	MenuClass = MenuBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/PerceptiveAI/UI/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr))
	{
		return;
	}
	InGameMenuClass = InGameMenuBPClass.Class;
}

void UMultiplayerGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Found class %s"), *MenuClass->GetName());
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!ensure(OnlineSubsystem != nullptr))
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Found OnlineSubsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString());

	SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!ensure(SessionInterface != nullptr))
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Found SessionInterface"));

	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnCreateSessionComplete);
	SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnDestroySessionComplete);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnFindSessionsComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnJoinSessionComplete);
}

void UMultiplayerGameInstance::LoadMainMenu(bool bLoadMainMenuLevel)
{
	if (bLoadMainMenuLevel)
	{
		APlayerController* PlayerController = GetFirstLocalPlayerController();
		if (!ensure(PlayerController != nullptr))
		{
			return;
		}
		PlayerController->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
		return;
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr))
	{
		return;
	}
	Engine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("LoadMainMenu"));

	if (!ensure(MenuClass != nullptr))
	{
		return;
	}
	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr))
	{
		return;
	}
	Menu->Setup();
	Menu->SetMenuInterface(this);
}

void UMultiplayerGameInstance::QuitGame()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		return;
	}
	PlayerController->ConsoleCommand("quit");
}

void UMultiplayerGameInstance::LoadInGameMenu()
{
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr))
	{
		return;
	}
	Engine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("LoadInGameMenu"));

	if (!ensure(InGameMenuClass != nullptr))
	{
		return;
	}
	UMenuWidget* InGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);
	if (!ensure(InGameMenu != nullptr))
	{
		return;
	}
	InGameMenu->Setup();
	InGameMenu->SetMenuInterface(this);
}

void UMultiplayerGameInstance::Host(const FString& ServerName)
{
	if (SessionInterface != nullptr)
	{
		auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("This session (%s) is already existed."), *ExistingSession->SessionName.ToString());
			UE_LOG(LogTemp, Warning, TEXT("Start DestroySession..."));
			SessionInterface->DestroySession(NAME_GameSession);
		}
		else
		{
			CreateSession(ServerName);
		}
	}
}

void UMultiplayerGameInstance::CreateSession(const FString& ServerName)
{
	if (SessionInterface != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Start CreateSession..."));
		FOnlineSessionSettings SessionSettings;

		IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
		if (OnlineSubsystem->GetSubsystemName().IsEqual(TEXT("NULL")))
		{
			UE_LOG(LogTemp, Warning, TEXT("bIsLANMatch: true"));
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("bIsLANMatch: false"));
			SessionSettings.bIsLANMatch = false;
		}

		SessionSettings.NumPublicConnections = 4;
		SessionSettings.bShouldAdvertise = true;	// display in the session list
		SessionSettings.bUsesPresence = true;	// true - CreateLobbySession, false - CreateInternetSession (check FOnlineSessionSteam::CreateSession)
		SessionSettings.Set(SESSION_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
	}
}

void UMultiplayerGameInstance::JoinWithIP(const FString& Address)
{
	//if (Menu != nullptr)
	//{
	//	Menu->Teardown();
	//}

	//UEngine* Engine = GetEngine();
	//if (!ensure(Engine != nullptr))
	//{
	//	return;
	//}
	//Engine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Joinning: %s"), *Address));

	//APlayerController* PlayerController = GetFirstLocalPlayerController();
	//if (!ensure(PlayerController != nullptr))
	//{
	//	return;
	//}
	//PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);

}

void UMultiplayerGameInstance::Join(uint32 Index)
{
	if (SessionInterface == nullptr)
	{
		return;
	}

	if (SessionSearch == nullptr)
	{
		return;
	}

	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

	UE_LOG(LogTemp, Warning, TEXT("Start Joinning..."));

	SessionInterface->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[Index]);
}

void UMultiplayerGameInstance::StartSession()
{
	if (SessionInterface != nullptr)
	{
		SessionInterface->StartSession(NAME_GameSession);	// So It's closed to join
	}
}

void UMultiplayerGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		return;

	}

	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete"));

	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

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
	World->ServerTravel(LOBBY_URL);
}

void UMultiplayerGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionComplete. Create a new Session"));
		CreateSession("Gwang's game");
	}
}

void UMultiplayerGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete. Result: %d"), Result);
	if (SessionInterface == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SessionInterface is null"));
		return;
	}

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("GetResolvedConnectString failed."));
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Joinning: %s"), *Address);
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UMultiplayerGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Start FindSessions..."));
		SessionSearch->bIsLanQuery = false;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UMultiplayerGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful && SessionSearch != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete"));
		TArray<FServerData> ServerNames;
		for (FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
		{
			FString Display = "";
			UE_LOG(LogTemp, Warning, TEXT("Session: %s."), *Result.GetSessionIdStr());

			FServerData Data;
			Data.SessionID = Result.GetSessionIdStr();
			Data.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - Result.Session.NumOpenPublicConnections;
			Data.HostUserName = Result.Session.OwningUserName;

			FString ServerName;
			if (Result.Session.SessionSettings.Get(SESSION_KEY, ServerName))
			{
				UE_LOG(LogTemp, Warning, TEXT("Data found in settings: %s."), *ServerName);
				Data.SessionID = ServerName;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Didn't get expected data"));
			}

			ServerNames.Add(Data);
		}

		if (Menu != nullptr)
		{
			Menu->SetServerList(ServerNames);
		}
	}
}
