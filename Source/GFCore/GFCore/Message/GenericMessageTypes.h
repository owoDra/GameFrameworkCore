// Copyright (C) 2023 owoDra

#pragma once

#include "GameplayTagContainer.h"

#include "GenericMessageTypes.generated.h"


/**
 * Represents a generic message of the form Instigator Verb Target (in Context, with Magnitude)
 */
USTRUCT(BlueprintType)
struct GFCORE_API FGenericVerbMessage
{
	GENERATED_BODY()
public:
	FGenericVerbMessage() {}

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag Verb;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UObject> Instigator{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UObject> Target{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer InstigatorTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer TargetTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer ContextTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double Magnitude{ 1.0 };

public:
	FString ToString() const;

};
