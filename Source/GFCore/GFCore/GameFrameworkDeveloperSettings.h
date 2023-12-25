﻿// Copyright (C) 2023 owoDra

#pragma once

#include "Engine/DeveloperSettings.h"

#include "GameFrameworkDeveloperSettings.generated.h"


/**
 * Settings for a Game framework.
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Game Framework Settings"))
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
	UPROPERTY(Config, EditAnywhere, Category = "Game Features", meta = (MustImplement = "GameFeatureStateChangeObserver"))
	TArray<FSoftClassPath> Observers;

};
