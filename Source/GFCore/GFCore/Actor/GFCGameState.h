// Copyright (C) 2024 owoDra

#pragma once

#include "GameFramework/GameState.h"

#include "GFCGameState.generated.h"


/** 
 * Pair this with a GFCGameModeBase 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	AGFCGameStateBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};


/** 
 * Pair this with a GFCGameState 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCGameState : public AGameState
{
	GENERATED_BODY()
public:
	AGFCGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void HandleMatchHasStarted() override;

};
