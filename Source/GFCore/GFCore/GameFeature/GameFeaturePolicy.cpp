// Copyright (C) 2024 owoDra

#include "GameFeature/GameFeaturePolicy.h"

#include "GameFrameworkDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeaturePolicy)


UGameFeaturePolicy::UGameFeaturePolicy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UGameFeaturePolicy::InitGameFeatureManager()
{
	// Load and create observers

	const auto* DevSettings{ GetDefault<UGameFrameworkDeveloperSettings>() };

	for (const auto& ObserverSoftClass : DevSettings->Observers)
	{
		if (!ObserverSoftClass.IsNull())
		{
			const auto* ObserverClass{ ObserverSoftClass.TryLoadClass<UObject>() };

			Observers.Add(NewObject<UObject>(this, ObserverClass));
		}
	}

	// Add observers to subsystem

	auto& Subsystem{ UGameFeaturesSubsystem::Get() };
	for (const auto& Observer : Observers)
	{
		Subsystem.AddObserver(Observer);
	}

	Super::InitGameFeatureManager();
}

void UGameFeaturePolicy::ShutdownGameFeatureManager()
{
	Super::ShutdownGameFeatureManager();

	auto& Subsystem{ UGameFeaturesSubsystem::Get() };
	for (const auto& Observer : Observers)
	{
		Subsystem.RemoveObserver(Observer);
	}

	Observers.Empty();
}

void UGameFeaturePolicy::GetGameFeatureLoadingMode(bool& bLoadClientData, bool& bLoadServerData) const
{
	// Editor will load both, this can cause hitching as the bundles are set to not preload in editor

	bLoadClientData = !IsRunningDedicatedServer();
	bLoadServerData = !IsRunningClientOnly();
}


UGameFeaturePolicy& UGameFeaturePolicy::Get()
{
	return UGameFeaturesSubsystem::Get().GetPolicy<UGameFeaturePolicy>();
}
