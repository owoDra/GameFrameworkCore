// Copyright (C) 2023 owoDra

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "GameplayTagContainer.h"

#include "InitStateSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GFCORE_API UInitStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UInitStateSubsystem() {}

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

};