// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayGameGameModeBase.h"

AMultiplayGameGameModeBase::AMultiplayGameGameModeBase()
{	
	// 
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/PerceptiveAI/Characters/Player/BP_Player"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
