// Copyright (C) 2024 owoDra

#pragma once

#include "GameFramework/PlayerController.h"

#include "GFCPlayerController.generated.h"


/** 
 * Minimal class that supports extension by game feature plugins 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AGFCPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//
	// Name of the event that signals that PlayerState is ready.
	//
	inline static const FName NAME_PlayerStateReady{ TEXTVIEW("PlayerStateReady") };

public:
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitPlayerState() override;
	virtual void OnRep_PlayerState() override;

public:	
	virtual void ReceivedPlayer() override;
	virtual void SetPawn(APawn* InPawn) override;
	virtual void OnPossess(class APawn* APawn) override;
	virtual void OnUnPossess() override;
	virtual void PlayerTick(float DeltaTime) override;

};
