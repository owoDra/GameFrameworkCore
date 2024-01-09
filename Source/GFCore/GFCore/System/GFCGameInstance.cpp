// Copyright (C) 2024 owoDra

#include "GFCGameInstance.h"

#include "InitState/InitStateTags.h"
#include "GFCoreLogs.h"

#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCGameInstance)


UGFCGameInstance::UGFCGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UGFCGameInstance::Init()
{
	Super::Init();

	auto* Subsystem{ GetSubsystem<UGameFrameworkComponentManager>() };

	// @TODO: Allow changes from DeveloperSetting

	static const TArray<FGameplayTag> StateChain
	{
		TAG_InitState_Spawned,
		TAG_InitState_DataAvailable,
		TAG_InitState_DataInitialized,
		TAG_InitState_GameplayReady
	};

	for (const auto& InitState : StateChain)
	{
		Subsystem->RegisterInitState(InitState, false, FGameplayTag());
	}
}


int32 UGFCGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	auto ReturnVal{ Super::AddLocalPlayer(NewPlayer, UserId) };

	if (ReturnVal != INDEX_NONE)
	{
		if (!PrimaryPlayer.IsValid())
		{
			UE_LOG(LogGFC, Log, TEXT("AddLocalPlayer: Set %s to Primary Player"), *NewPlayer->GetName());

			PrimaryPlayer = NewPlayer;
		}

		OnLocalPlayerAdded.Broadcast(NewPlayer, UserId);
	}

	return ReturnVal;
}

bool UGFCGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	if (PrimaryPlayer == ExistingPlayer)
	{
		//@TODO: do we want to fall back to another player?

		PrimaryPlayer.Reset();

		UE_LOG(LogGFC, Log, TEXT("RemoveLocalPlayer: Unsetting Primary Player from %s"), *ExistingPlayer->GetName());
	}
	
	OnLocalPlayerRemoved.Broadcast(ExistingPlayer);

	return Super::RemoveLocalPlayer(ExistingPlayer);
}

void UGFCGameInstance::ReturnToMainMenu()
{
	OnReturnToMainMenu.Broadcast();

	Super::ReturnToMainMenu();
}


FDelegateHandle UGFCGameInstance::Register_OnLocalPlayerAdded(FLocalPlayerAddedDelegate::FDelegate Delegate)
{
	if (Delegate.IsBound())
	{
		return OnLocalPlayerAdded.Add(Delegate);
	}

	return FDelegateHandle();
}

FDelegateHandle UGFCGameInstance::Register_OnLocalPlayerRemoved(FLocalPlayerRemovedDelegate::FDelegate Delegate)
{
	if (Delegate.IsBound())
	{
		return OnLocalPlayerRemoved.Add(Delegate);
	}

	return FDelegateHandle();
}

FDelegateHandle UGFCGameInstance::Register_OnReturnToMainMenu(FReturnToMainMenuDelegate::FDelegate Delegate)
{
	if (Delegate.IsBound())
	{
		return OnReturnToMainMenu.Add(Delegate);
	}

	return FDelegateHandle();
}
