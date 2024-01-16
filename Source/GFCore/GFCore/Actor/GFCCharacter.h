// Copyright (C) 2024 owoDra

#pragma once

#include "GameFramework/Character.h"
#include "Character/CharacterMeshAccessorInterface.h"

#include "GFCCharacter.generated.h"


/** 
 * Minimal class that supports extension by game feature plugins 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCCharacter 
	: public ACharacter
	, public ICharacterMeshAccessorInterface
{
	GENERATED_BODY()
public:
	AGFCCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	////////////////////////////////////////////////
	// Mesh Accessor
public:
	TArray<USkeletalMeshComponent*> GetMeshes_Implementation() const override;
	USkeletalMeshComponent* GetMainMesh_Implementation() const override;
	USkeletalMeshComponent* GetMeshByTag_Implementation(const FGameplayTag& Tag) const override;

};
