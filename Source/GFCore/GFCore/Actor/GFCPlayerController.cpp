// Copyright (C) 2023 owoDra

#include "GFCPlayerController.h"

#include "Components/ControllerComponent.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCPlayerController)


void AGFCPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGFCPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AGFCPlayerController::ReceivedPlayer()
{
	// Player controllers always get assigned a player and can't do much until then

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::ReceivedPlayer();

	TArray<UControllerComponent*> GFCComponents;
	GetComponents(GFCComponents);
	for (const auto& Component : GFCComponents)
	{
		Component->ReceivedPlayer();
	}
}

void AGFCPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	TArray<UControllerComponent*> GFCComponents;
	GetComponents(GFCComponents);
	for (const auto& Component : GFCComponents)
	{
		Component->PlayerTick(DeltaTime);
	}
}
