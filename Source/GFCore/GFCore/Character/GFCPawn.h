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
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
