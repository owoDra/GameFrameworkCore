// Copyright (C) 2023 owoDra

#include "InitStateSubsystem.h"

#include "InitStateTags.h"

#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InitStateSubsystem)


void UInitStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	auto* Subsystem{ Collection.InitializeDependency(UGameFrameworkComponentManager::StaticClass()) };
	auto* GFCM{ CastChecked<UGameFrameworkComponentManager>(Subsystem) };

	// @TODO: Allow changes from DeveloperSetting

	static const TArray<FGameplayTag> StateChain
	{
		TAG_InitState_Spawned,
		TAG_InitState_DataAvailable,
		TAG_InitState_DataInitialized,
		TAG_InitState_GameplayReady
	};

	for (const auto& InitState : StateChain)
	{
		GFCM->RegisterInitState(InitState, false, FGameplayTag::EmptyTag);
	}
}
