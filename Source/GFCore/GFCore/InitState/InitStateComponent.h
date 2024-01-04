// Copyright (C) 2023 owoDra

#pragma once

#include "Components/GameFrameworkComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"

#include "Delegates/Delegate.h"

#include "InitStateComponent.generated.h"


/**
 * Component that manages the initialization state of Actor
 * 
 * Tips:
 *	Through this component, the initialization progress of all of Actor's features can be monitored.
 *  If there is data required to initialize all functions of Actor, this component should be inherited and implemented.
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class GFCORE_API UInitStateComponent
	: public UGameFrameworkComponent
	, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UInitStateComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//
	// Function name used to add this component
	//
	static const FName NAME_ActorFeatureName;

	static const FName NAME_InitStateComplete;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;

protected:
	virtual bool CanChangeInitStateToSpawned(UGameFrameworkComponentManager* Manager) const { return true; }
	virtual bool CanChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager) const { return true; }
	virtual bool CanChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager) const { return true; }
	virtual bool CanChangeInitStateToGameplayReady(UGameFrameworkComponentManager* Manager) const { return true; }

	virtual void HandleChangeInitStateToSpawned(UGameFrameworkComponentManager* Manager) {}
	virtual void HandleChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager) {}
	virtual void HandleChangeInitStateToDataInitialized(UGameFrameworkComponentManager* Manager) {}
	virtual void HandleChangeInitStateToGameplayReady(UGameFrameworkComponentManager* Manager) {}


protected:
	//
	// Delegate notifying that initialization process has been completed.
	//
	FSimpleMulticastDelegate OnGameReadyDelegate;

public:
	/**
	 * Register a callback to inform completion of initialization
	 */
	void OnGameReady_Register(FSimpleMulticastDelegate::FDelegate Delegate);

};
