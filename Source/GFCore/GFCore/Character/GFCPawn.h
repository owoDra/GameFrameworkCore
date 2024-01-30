// Copyright (C) 2024 owoDra

#pragma once

#include "GameFramework/Pawn.h"

#include "GFCPawn.generated.h"


/** 
 * Minimal class that supports extension by game feature plugins 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCPawn : public APawn
{
	GENERATED_BODY()

public:
	//
	// Name of the event that signals that PlayerState is ready.
	//
	inline static const FName NAME_PlayerStateReady{ TEXTVIEW("PlayerStateReady") };

public:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;
};
