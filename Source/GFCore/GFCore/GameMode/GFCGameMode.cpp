// Copyright (C) 2024 owoDra

#include "GFCGameMode.h"

#include "GameMode/GFCGameState.h"
#include "Character/GFCPawn.h"
#include "Player/GFCPlayerController.h"
#include "Player/GFCPlayerState.h"

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
