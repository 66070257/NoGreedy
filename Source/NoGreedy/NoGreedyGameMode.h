// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NoGreedyGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ANoGreedyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	ANoGreedyGameMode();
};



