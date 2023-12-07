// Copyright (C) 2023 owoDra

#pragma once

#include "AIController.h"

#include "GFCAIController.generated.h"


/** 
 * Minimal class that supports extension by game feature plugins 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
