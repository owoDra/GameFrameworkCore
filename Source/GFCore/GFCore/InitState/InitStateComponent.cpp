// Copyright (C) 2023 owoDra

#include "InitState/InitStateComponent.h"

#include "InitState/InitStateTags.h"

#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InitStateComponent)


const FName UInitStateComponent::NAME_ActorFeatureName("InitState");

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
	GetOwner()->GetComponents(UInitStateComponent::StaticClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one InitStateComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

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

	ensure(TryToChangeInitState(TAG_InitState_Spawned));

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

	auto* Owner{ GetOwner() };

	/**
	 * [None] -> [Spawned]
	 * 
	 */
	if (!CurrentState.IsValid() && DesiredState == TAG_InitState_Spawned)
	{
		// Check Owner

		if (Owner)
		{
			return true;
		}
	}

	/**
	 * [Spawned] -> [DataAvailable]
	 * 
	 * Tips:
	 *	At any time that the current state is valid (Spawned or later), 
	 *	an attempt can be made to move to this state.
	 * 
	 *  When features are changed or a new feature is added to actor later,
	 *	it is moved to this state to reset the initialization state.
	 */
	if (CurrentState.IsValid() && DesiredState == TAG_InitState_DataAvailable)
	{
		return true;
	}

	/**
	 * [DataAvailable] -> [DataInitialized]
	 */
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		// Whether all other functions of Owner that own this component have reached [DataInitialized]

		return Manager->HaveAllFeaturesReachedInitState(Owner, TAG_InitState_DataInitialized, NAME_ActorFeatureName);
	}

	/**
	 * [DataInitialized] -> [GameplayReady]
	 */
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		// Whether all other functions of Owner that own this component have reached [GameplayReady]

		return Manager->HaveAllFeaturesReachedInitState(Owner, TAG_InitState_GameplayReady, NAME_ActorFeatureName);
	}

	return false;
}

void UInitStateComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	/**
	 * [DataInitialized] -> [GameplayReady]
	 */
	if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		OnGameReadyDelegate.Broadcast();
	}
}

void UInitStateComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// Executed when the initialization state is changed 
	// by a feature other than this component of the actor that owns this component.

	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		/**
		 * -> [Spawned]
		 * 
		 * Tips:
		 *	Rewind the initialization state of this component to [DataAvailable] 
		 *	when other functions reach [Spawned]
		 * 
		 *  To be able to understand the initialization state of 
		 *	the entire features through this component.
		 */
		if (Params.FeatureState == TAG_InitState_Spawned)
		{
			ensure(TryToChangeInitState(TAG_InitState_DataAvailable));

			CheckDefaultInitialization();
		}

		/**
		 * -> [DataInitialized]
		 */
		else if (Params.FeatureState == TAG_InitState_DataInitialized)
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
