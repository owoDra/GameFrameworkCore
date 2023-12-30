// Copyright (C) 2023 owoDra

#pragma once

#include "Engine/DeveloperSettings.h"

#include "GameFrameworkDeveloperSettings.generated.h"


/**
 * Settings for a Game framework.
 */
UCLASS(Config = "Game", Defaultconfig, meta = (DisplayName = "Game Framework Core"))
class UGameFrameworkDeveloperSettings : public UDeveloperSettings
{
public:
	GENERATED_BODY()
public:
	UGameFrameworkDeveloperSettings();

	///////////////////////////////////////////////
	// Game Features
public:
	//
	// List of Observer classes to be created in GameFeaturePolicy
	//
	UPROPERTY(Config, EditAnywhere, Category = "Game Features", meta = (MustImplement = "/Script/GameFeatures.GameFeatureStateChangeObserver"))
	TArray<FSoftClassPath> Observers;

};

