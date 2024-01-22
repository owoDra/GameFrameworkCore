// Copyright (C) 2024 owoDra

#include "Actor/GFCPlayerController.h"

#include "Actor/GFCLocalPlayer.h"

#include "Components/ControllerComponent.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCPlayerController)


const FName AGFCPlayerController::NAME_PlayerStateReady("PlayerStateReady");

AGFCPlayerController::AGFCPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


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


void AGFCPlayerController::InitPlayerState()
{
	Super::InitPlayerState();

	if (GetNetMode() != NM_Client)
	{
		UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, AGFCPlayerController::NAME_PlayerStateReady);
	}
}

void AGFCPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (GetNetMode() == NM_Client)
	{
		UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, AGFCPlayerController::NAME_PlayerStateReady);
	}
}


void AGFCPlayerController::ReceivedPlayer()
{
	// Player controllers always get assigned a player and can't do much until then

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::ReceivedPlayer();

	// Notify to Controller Components

	TArray<UControllerComponent*> GFCComponents;
	GetComponents(GFCComponents);
	for (const auto& Component : GFCComponents)
	{
		Component->ReceivedPlayer();
	}

	// Notify to Local Player

	if (auto* LocalPlayer{ Cast<UGFCLocalPlayer>(Player) })
	{
		LocalPlayer->OnPlayerControllerSet.Broadcast(LocalPlayer, this);

		if (PlayerState)
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}

void AGFCPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (auto* LocalPlayer{ Cast<UGFCLocalPlayer>(Player) })
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, InPawn);
	}
}

void AGFCPlayerController::OnPossess(APawn* APawn)
{
	Super::OnPossess(APawn);

	if (auto* LocalPlayer{ Cast<UGFCLocalPlayer>(Player) })
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, APawn);
	}
}

void AGFCPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (auto* LocalPlayer{ Cast<UGFCLocalPlayer>(Player) })
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, nullptr);
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
