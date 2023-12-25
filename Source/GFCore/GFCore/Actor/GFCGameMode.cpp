// Copyright (C) 2023 owoDra

#include "GFCGameMode.h"

#include "GFCGameState.h"
#include "GFCPawn.h"
#include "Actor/GFCPlayerController.h"
#include "GFCPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCGameMode)


#pragma region GameModeBase

AGFCGameModeBase::AGFCGameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AGFCGameStateBase::StaticClass();
	PlayerControllerClass = AGFCPlayerController::StaticClass();
	PlayerStateClass = AGFCPlayerState::StaticClass();
	DefaultPawnClass = AGFCPawn::StaticClass();
}


void AGFCGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	OnGameModeCombinedPostLogin.Broadcast(this, NewPlayer);
}

#pragma endregion


#pragma region GameMode

AGFCGameMode::AGFCGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AGFCGameState::StaticClass();
	PlayerControllerClass = AGFCPlayerController::StaticClass();
	PlayerStateClass = AGFCPlayerState::StaticClass();
	DefaultPawnClass = AGFCPawn::StaticClass();
}


void AGFCGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	OnGameModeCombinedPostLogin.Broadcast(this, NewPlayer);
}

#pragma endregion
