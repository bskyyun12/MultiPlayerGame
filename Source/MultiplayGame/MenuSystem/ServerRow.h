// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 *
 */
UCLASS()
class MULTIPLAYGAME_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	class UTextBlock* GetSessionId() { return SessionId; }
	class UTextBlock* GetHostUserName() { return HostUserName; }
	class UTextBlock* GetServerCapacityInfo() { return ServerCapacityInfo; }

	void Setup(class UMainMenu* Parent, uint32 Index);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsSelected() { return bisSelected; }

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* ServerButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SessionId;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostUserName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerCapacityInfo;

	UPROPERTY()
	class UMainMenu* Parent;
	
	uint32 Index;

	bool bisSelected;

	UFUNCTION()
	void OnButtonClicked();
};
