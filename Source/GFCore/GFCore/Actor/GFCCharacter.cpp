// Copyright (C) 2024 owoDra

#include "GFCCharacter.h"

#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCCharacter)


AGFCCharacter::AGFCCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}


void AGFCCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGFCCharacter::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AGFCCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}


TArray<USkeletalMeshComponent*> AGFCCharacter::GetMeshes_Implementation() const
{
	TArray<USkeletalMeshComponent*> Result;

	Result.Emplace(GetMesh());

	return Result;
}

USkeletalMeshComponent* AGFCCharacter::GetMainMesh_Implementation() const
{
	return GetMesh();
}

USkeletalMeshComponent* AGFCCharacter::GetMeshByTag_Implementation(const FGameplayTag& Tag) const
{
	return GetMesh();
}
