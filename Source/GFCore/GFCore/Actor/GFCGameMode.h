// Copyright (C) 2023 owoDra

#pragma once

#include "GameFramework/GameMode.h"

#include "GFCGameMode.generated.h"


/** 
 * Pair this with a GFCGameStateBase 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGFCGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};


/** 
 * Pair this with a GFCGameState 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AGFCGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};
