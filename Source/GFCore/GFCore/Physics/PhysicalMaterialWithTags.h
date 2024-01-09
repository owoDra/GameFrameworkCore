// Copyright (C) 2024 owoDra

#pragma once

#include "PhysicalMaterials/PhysicalMaterial.h"

#include "GameplayTagContainer.h"

#include "PhysicalMaterialWithTags.generated.h"


/**
 * PhyscalMaterial class that can register GameplayTag
 */
UCLASS()
class GFCORE_API UPhysicalMaterialWithTags : public UPhysicalMaterial
{
	GENERATED_BODY()
public:
	UPhysicalMaterialWithTags(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// A container of gameplay tags that game code can use to reason about this physical material
	//
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PhysicalProperties")
	FGameplayTagContainer Tags;

};
