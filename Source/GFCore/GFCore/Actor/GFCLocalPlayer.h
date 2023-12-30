// Copyright (C) 2023 owoDra

#pragma once

#include "Engine/LocalPlayer.h"

#include "GFCLocalPlayer.generated.h"


UCLASS(Transient)
class GFCORE_API UGFCLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()
public:
	UGFCLocalPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	/** 
	 * Called when the local player is assigned a player controller 
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UGFCLocalPlayer* LocalPlayer, APlayerController* PlayerController);
	FPlayerControllerSetDelegate OnPlayerControllerSet;

	/** 
	 * Called when the local player is assigned a player state 
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UGFCLocalPlayer* LocalPlayer, APlayerState* PlayerState);
	FPlayerStateSetDelegate OnPlayerStateSet;

	/** 
	 * Called when the local player is assigned a player pawn 
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerPawnSetDelegate, UGFCLocalPlayer* LocalPlayer, APawn* Pawn);
	FPlayerPawnSetDelegate OnPlayerPawnSet;

public:
	FDelegateHandle CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate);
	FDelegateHandle CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate);
	FDelegateHandle CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate);


private:
	bool bIsPlayerViewEnabled{ true };

public:
	bool IsPlayerViewEnabled() const { return bIsPlayerViewEnabled; }
	void SetIsPlayerViewEnabled(bool bInIsPlayerViewEnabled) { bIsPlayerViewEnabled = bInIsPlayerViewEnabled; }

	virtual bool GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData, int32 StereoViewIndex) const override;

};
