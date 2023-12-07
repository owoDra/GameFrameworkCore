// Copyright (C) 2023 owoDra

#include "GFCPlayerState.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Components/PlayerStateComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCPlayerState)


void AGFCPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGFCPlayerState::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AGFCPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AGFCPlayerState::Reset()
{
	Super::Reset();

	TArray<UPlayerStateComponent*> GFCComponents;
	GetComponents(GFCComponents);
	for (const auto& Component : GFCComponents)
	{
		Component->Reset();
	}
}


void AGFCPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	TInlineComponentArray<UPlayerStateComponent*> PlayerStateComponents;
	GetComponents(PlayerStateComponents);
	for (const auto& SourcePSComp : PlayerStateComponents)
	{
		if (auto* TargetComp = Cast<UPlayerStateComponent>(static_cast<UObject*>(FindObjectWithOuter(PlayerState, SourcePSComp->GetClass(), SourcePSComp->GetFName()))))
		{
			SourcePSComp->CopyProperties(TargetComp);
		}
	}
}
