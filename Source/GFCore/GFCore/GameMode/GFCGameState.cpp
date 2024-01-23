// Copyright (C) 2024 owoDra

#include "GFCGameState.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Components/GameStateComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCGameState)


#pragma region GameStateBase

AGFCGameStateBase::AGFCGameStateBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void AGFCGameStateBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGFCGameStateBase::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AGFCGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

#pragma endregion


#pragma region GameState

AGFCGameState::AGFCGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void AGFCGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGFCGameState::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AGFCGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}


void AGFCGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	TArray<UGameStateComponent*> GFCComponents;
	GetComponents(GFCComponents);
	for (const auto& Component : GFCComponents)
	{
		Component->HandleMatchHasStarted();
	}
}

#pragma endregion
