// Copyright (C) 2023 owoDra

#include "GFCPlayerStateComponent.h"

#include "InitState/InitStateTags.h"
#include "InitState/InitStateComponent.h"
#include "GFCoreLogs.h"

#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCPlayerStateComponent)


UGFCPlayerStateComponent::UGFCPlayerStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UGFCPlayerStateComponent::OnRegister()
{
	Super::OnRegister();

	// This component can only be added to classes derived from APlayerState

	const auto* PlayerState{ GetPlayerState<APlayerState>() };
	ensureAlwaysMsgf((PlayerState != nullptr), TEXT("[%s] on [%s] can only be added to PlayerState actors."), *GetNameSafe(StaticClass()), *GetNameSafe(GetOwner()));

	// No more than two of these components should be added to a Actor.

	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(StaticClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one [%s] should exist on [%s]."), *GetNameSafe(StaticClass()), *GetNameSafe(GetOwner()));

	// Register this component in the GameFrameworkComponentManager.

	RegisterInitStateFeature();
}

void UGFCPlayerStateComponent::BeginPlay()
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

void UGFCPlayerStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}


bool UGFCPlayerStateComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
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

void UGFCPlayerStateComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
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

void UGFCPlayerStateComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UInitStateComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == TAG_InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UGFCPlayerStateComponent::CheckDefaultInitialization()
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
