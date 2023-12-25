// Copyright (C) 2023 owoDra

#pragma once

#include "GameFeatureAction.h"

#include "GameFeaturesSubsystem.h"

#include "GameFeatureAction_WorldActionBase.generated.h"

class FDelegateHandle;
class UGameInstance;


/**
 * Base class for GameFeatureActions that wish to do something world specific.
 */
UCLASS(Abstract)
class GFCORE_API UGameFeatureAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()
public:
	UGameFeatureAction_WorldActionBase() {}

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;


private:
	TMap<FGameFeatureStateChangeContext, FDelegateHandle> GameInstanceStartHandles;

private:
	void HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext);

public:
	/** 
	 * Override with the action-specific logic 
	 */
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) PURE_VIRTUAL(UGameFeatureAction_WorldActionBase::AddToWorld,);

};
