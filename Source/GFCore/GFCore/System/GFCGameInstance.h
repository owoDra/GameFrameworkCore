// Copyright (C) 2023 owoDra

#pragma once

#include "Engine/GameInstance.h"

#include "GFCGameInstance.generated.h"


UCLASS(Blueprintable)
class GFCORE_API UGFCGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UGFCGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	//
	// This is the primary player
	//
	TWeakObjectPtr<ULocalPlayer> PrimaryPlayer;

public:
	virtual void Init() override;

	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId) override;
	virtual bool RemoveLocalPlayer(ULocalPlayer* ExistingPlayer) override;
	virtual void ReturnToMainMenu() override;


public:
	/**
	 * Called when the local player added
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FLocalPlayerAddedDelegate, ULocalPlayer* NewPlayer, FPlatformUserId UserId);
	FLocalPlayerAddedDelegate OnLocalPlayerAdded;

	/**
	 * Called when the local player removed
	 */
	DECLARE_MULTICAST_DELEGATE_OneParam(FLocalPlayerRemovedDelegate, ULocalPlayer* ExistingPlayer);
	FLocalPlayerRemovedDelegate OnLocalPlayerRemoved;

	/**
	 * Called when returning to main menu
	 */
	DECLARE_MULTICAST_DELEGATE(FReturnToMainMenuDelegate);
	FReturnToMainMenuDelegate OnReturnToMainMenu;

public:
	FDelegateHandle Register_OnLocalPlayerAdded(FLocalPlayerAddedDelegate::FDelegate Delegate);
	FDelegateHandle Register_OnLocalPlayerRemoved(FLocalPlayerRemovedDelegate::FDelegate Delegate);
	FDelegateHandle Register_OnReturnToMainMenu(FReturnToMainMenuDelegate::FDelegate Delegate);

};
