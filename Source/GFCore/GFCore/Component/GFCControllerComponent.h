// Copyright (C) 2024 owoDra

#pragma once

#include "Components/ControllerComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"

#include "GFCControllerComponent.generated.h"

/**
 * ControllerComponent with basic initialization flow processing implemented
 */
UCLASS()
class GFCORE_API UGFCControllerComponent 
	: public UControllerComponent
	, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UGFCControllerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
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

};
