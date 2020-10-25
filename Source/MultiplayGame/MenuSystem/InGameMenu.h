// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"

#include "InGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYGAME_API UInGameMenu : public UMenuWidget
{
	GENERATED_BODY()	

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;


	UFUNCTION()
	void OnCancelButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();
};
