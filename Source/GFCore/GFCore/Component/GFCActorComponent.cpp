// Copyright (C) 2023 owoDra

#include "GFCActorComponent.h"

#include "InitState/InitStateTags.h"
#include "InitState/InitStateComponent.h"
#include "GFCoreLogs.h"

#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCActorComponent)


UGFCActorComponent::UGFCActorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UGFCActorComponent::OnRegister()
{
	Super::OnRegister();

	// No more than two of these components should be added to a Actor.

	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(GetClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one [%s] should exist on [%s]."), *GetNameSafe(GetClass()), *GetNameSafe(GetOwner()));

	// Register this component in the GameFrameworkComponentManager.

	RegisterInitStateFeature();
}

void UGFCActorComponent::BeginPlay()
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

void UGFCActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}


bool UGFCActorComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	/**
	 * [InitState None] -> [InitState Spawned]
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
		if (!Manager->HasFeatureReachedInitState(GetOwner(), UInitStateComponent::NAME_ActorFeatureName, TAG_InitState_DataAvailable))
		{
			return false;
		}

		return CanChangeInitStateToDataAvailable(Manager);
	}

	/**
	 * [InitState DataAvailable] -> [InitState DataInitialized]
	 */
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		return CanChangeInitStateToDataInitialized(Manager);
	}

	/**
	 * [InitState DataInitialized] -> [InitState GameplayReady]
	 */
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		return CanChangeInitStateToGameplayReady(Manager);
	}

	return false;
}

void UGFCActorComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	check(Manager);

	UE_LOG(LogGFC, Log, TEXT("[%s] %s: InitState Reached: %s"),
		GetOwner()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"),
		*GetNameSafe(this),
		*DesiredState.GetTagName().ToString());

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
		HandleChangeInitStateToGameplayReady(Manager);
	}
}

void UGFCActorComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UInitStateComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == TAG_InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UGFCActorComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain
	{
		TAG_InitState_Spawned,
		TAG_InitState_DataAvailable,
		TAG_InitState_DataInitialized,
		TAG_InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}
