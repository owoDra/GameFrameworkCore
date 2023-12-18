// Copyright (C) 2023 owoDra

#pragma once

#include "GameFramework/GameMode.h"

#include "GFCGameMode.generated.h"


/**
 * Post login event, triggered when a player joins the game as well as after non-seamless ServerTravel
 *
 * This is called after the player has finished initialization
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FGameModeCombinedPostLoginDelegate, AGameModeBase* /*GameMode*/, AController* /*NewPlayer*/);


/** 
 * Pair this with a GFCGameStateBase 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGFCGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	FGameModeCombinedPostLoginDelegate OnGameModeCombinedPostLogin;

protected:
	virtual void OnPostLogin(AController* NewPlayer) override;

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

public:
	FGameModeCombinedPostLoginDelegate OnGameModeCombinedPostLogin;

protected:
	virtual void OnPostLogin(AController* NewPlayer) override;

};
