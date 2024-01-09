// Copyright (C) 2024 owoDra

#pragma once

#include "GameFramework/Character.h"

#include "GFCCharacter.generated.h"


/** 
 * Minimal class that supports extension by game feature plugins 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	AGFCCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
