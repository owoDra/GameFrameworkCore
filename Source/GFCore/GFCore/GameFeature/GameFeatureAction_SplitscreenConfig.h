// Copyright (C) 2024 owoDra

#pragma once

#include "GameFeatureAction_WorldActionBase.h"

#include "GameFeatureAction_SplitscreenConfig.generated.h"


/**
 * GameFeatureAction responsible for granting abilities (and attributes) to actors of a specified type.
 */
UCLASS(meta = (DisplayName = "Splitscreen Config"))
class UGameFeatureAction_SplitscreenConfig final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	UGameFeatureAction_SplitscreenConfig() {}

public:
	UPROPERTY(EditAnywhere, Category = Action)
	bool bDisableSplitscreen{ true };

public:
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;


private:
	TArray<FObjectKey> LocalDisableVotes;

	static TMap<FObjectKey, int32> GlobalDisableVotes;

public:
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

};
