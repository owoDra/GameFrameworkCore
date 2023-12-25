// Copyright (C) 2023 owoDra

#pragma once

#include "GameFeaturesProjectPolicies.h"

#include "GameFeatureStateChangeObserver.h"

#include "GameFeaturePolicy.generated.h"

class UGameFeatureData;
struct FPrimaryAssetId;


/**
 * Manager to keep track of the state machines that bring a game feature plugin into memory and active
 * This class discovers plugins either that are built-in and distributed with the game or are reported externally (i.e. by a web service or other endpoint)
 */
UCLASS(MinimalAPI)
class UGameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
{
	GENERATED_BODY()
public:
	UGameFeaturePolicy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Observers;

public:
	virtual void InitGameFeatureManager() override;
	virtual void ShutdownGameFeatureManager() override;
	virtual void GetGameFeatureLoadingMode(bool& bLoadClientData, bool& bLoadServerData) const override;


public:
	GFCORE_API static UGameFeaturePolicy& Get();

};
