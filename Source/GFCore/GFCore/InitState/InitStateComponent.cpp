// Copyright (C) 2024 owoDra

#include "InitState/InitStateComponent.h"

#include "InitState/InitStateTags.h"

#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InitStateComponent)


const FName UInitStateComponent::NAME_ActorFeatureName("InitState");

const FName  UInitStateComponent::NAME_InitStateComplete("InitStateComplete");

UInitStateComponent::UInitStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}


void UInitStateComponent::OnRegister()
{
	Super::OnRegister();

	// No more than two of these components should be added to a Pawn.

	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(StaticClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one [%s] should exist on [%s]."), *GetNameSafe(StaticClass()), *GetNameSafe(GetOwner()));

	// Register this component in the GameFrameworkComponentManager.

	RegisterInitStateFeature();
}

void UInitStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// Start listening for changes in the initialization state of all features 
	// related to the Pawn that owns this component.

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Change the initialization state of this component to [Spawned]

	ensureMsgf(TryToChangeInitState(TAG_InitState_Spawned), TEXT("[%s] on [%s]."), *GetNameSafe(this), *GetNameSafe(GetOwner()));

	// Check if initialization process can continue

	CheckDefaultInitialization();
}

void UInitStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}


bool UInitStateComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	/**
	 * [None] -> [Spawned]
	 */
	if (!CurrentState.IsValid() && DesiredState == TAG_InitState_Spawned)
	{
		if (GetOwner() == nullptr)
		{
			return false;
		}

		return CanChangeInitStateToSpawned(Manager);
	}

	/**
	 * [InitState Spawned] -> [InitState DataAvailable]
	 */
	else if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
	{
		return CanChangeInitStateToDataAvailable(Manager);
	}

	/**
	 * [DataAvailable] -> [DataInitialized]
	 */
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		if (!Manager->HaveAllFeaturesReachedInitState(GetOwner(), TAG_InitState_DataInitialized, NAME_ActorFeatureName))
		{
			return false;
		}

		return CanChangeInitStateToDataInitialized(Manager);
	}

	/**
	 * [DataInitialized] -> [GameplayReady]
	 */
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		return CanChangeInitStateToGameplayReady(Manager);
	}

	return false;
}

void UInitStateComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	check(Manager);

	/**
	 * [InitState None] -> [InitState Spawned]
	 */
	if (!CurrentState.IsValid() && DesiredState == TAG_InitState_Spawned)
	{
		HandleChangeInitStateToSpawned(Manager);
	}

	/**
	 * [InitState Spawned] -> [InitState DataAvailable]
	 */
	else if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
	{
		HandleChangeInitStateToDataAvailable(Manager);
	}

	/**
	 * [InitState DataAvailable] -> [InitState DataInitialized]
	 */
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		HandleChangeInitStateToDataInitialized(Manager);
	}

	/**
	 * [InitState DataInitialized] -> [InitState GameplayReady]
	 */
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		OnGameReadyDelegate.Broadcast();

		UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(GetOwner(), NAME_InitStateComplete);

		HandleChangeInitStateToGameplayReady(Manager);
	}
}

void UInitStateComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// Executed when the initialization state is changed 
	// by a feature other than this component of the actor that owns this component.

	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		/**
		 * -> [DataInitialized]
		 */
		if (Params.FeatureState == TAG_InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}

		/**
		 * -> [GameplayReady]
		 */
		else if (Params.FeatureState == TAG_InitState_GameplayReady)
		{
			CheckDefaultInitialization();
		}
	}
}

void UInitStateComponent::CheckDefaultInitialization()
{
	// Perform initialization state checks on other features before checking the initialization state of this component

	CheckDefaultInitializationForImplementers();

	// @TODO: Allow changes from DeveloperSetting

	static const TArray<FGameplayTag> StateChain
	{
		TAG_InitState_Spawned,
		TAG_InitState_DataAvailable,
		TAG_InitState_DataInitialized,
		TAG_InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}


void UInitStateComponent::OnGameReady_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnGameReadyDelegate.IsBoundToObject(Delegate.GetUObject()))
	{
		OnGameReadyDelegate.Add(Delegate);
	}
}
