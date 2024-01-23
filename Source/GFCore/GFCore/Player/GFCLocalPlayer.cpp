// Copyright (C) 2024 owoDra

#include "GFCLocalPlayer.h"

#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCLocalPlayer)


UGFCLocalPlayer::UGFCLocalPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


FDelegateHandle UGFCLocalPlayer::CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate)
{
	if (auto* PC{ GetPlayerController(GetWorld()) })
	{
		Delegate.Execute(this, PC);
	}

	return OnPlayerControllerSet.Add(Delegate);
}

FDelegateHandle UGFCLocalPlayer::CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate)
{
	auto* PC{ GetPlayerController(GetWorld()) };

	if (auto PlayerState{ PC ? PC->PlayerState : nullptr })
	{
		Delegate.Execute(this, PlayerState);
	}
	
	return OnPlayerStateSet.Add(Delegate);
}

FDelegateHandle UGFCLocalPlayer::CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate)
{
	auto* PC{ GetPlayerController(GetWorld()) };

	if (auto* Pawn{ PC ? PC->GetPawn() : nullptr })
	{
		Delegate.Execute(this, Pawn);
	}

	return OnPlayerPawnSet.Add(Delegate);
}


bool UGFCLocalPlayer::GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData, int32 StereoViewIndex) const
{
	if (!bIsPlayerViewEnabled)
	{
		return false;
	}

	return Super::GetProjectionData(Viewport, ProjectionData, StereoViewIndex);
}
