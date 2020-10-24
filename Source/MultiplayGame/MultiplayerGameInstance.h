// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MultiplayerGameInstance.generated.h"

/**
 *
 */
UCLASS()
class MULTIPLAYGAME_API UMultiplayerGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMultiplayerGameInstance(const FObjectInitializer& ObjectIn);

	virtual void Init();

	UFUNCTION(Exec)
	void Host();

	UFUNCTION(Exec)
	void Join(const FString& Address);
};
