// Copyright (C) 2024 owoDra

#pragma once

#include "GameFramework/PlayerState.h"

#include "GFCPlayerState.generated.h"


/** 
 * Minimal class that supports extension by game feature plugins 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;

protected:
	virtual void CopyProperties(APlayerState* PlayerState);

};
