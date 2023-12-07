// Copyright (C) 2023 owoDra

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
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;

};
