// Copyright (C) 2024 owoDra

#pragma once

#include "UObject/Interface.h"

#include "GameplayTagContainer.h"

#include "CharacterMeshAccessorInterface.generated.h"

class USkeletalMeshComponent;


/**
 * Interface to assist with Pawn or Character SkeltalMesh
 */
UINTERFACE(BlueprintType)
class GFCORE_API UCharacterMeshAccessorInterface : public UInterface
{
	GENERATED_BODY()
public:
	UCharacterMeshAccessorInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};

class GFCORE_API ICharacterMeshAccessorInterface
{
	GENERATED_BODY()

public:
	/**
	 * Get all the Mesh that represents the appearance of the Pawn or Character.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mesh")
	TArray<USkeletalMeshComponent*> GetMeshes() const;

	/**
	 * Get main Mesh of Pawn or Character
	 * 
	 * Tips:
	 *	Basically, the mesh holding the main AnimInstance that is not CopyPose, etc. is returned.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mesh")
	USkeletalMeshComponent* GetMainMesh() const;

	/**
	 * Returns a mesh for a specific tag
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mesh")
	USkeletalMeshComponent* GetMeshByTag(const FGameplayTag& Tag) const;

};
