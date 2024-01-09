// Copyright (C) 2024 owoDra

#include "GameFeatureAction_SplitscreenConfig.h"

#include "GameFeaturesSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_SplitscreenConfig)


TMap<FObjectKey, int32> UGameFeatureAction_SplitscreenConfig::GlobalDisableVotes;

void UGameFeatureAction_SplitscreenConfig::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	for (auto i{ LocalDisableVotes.Num() - 1 }; i >= 0; i--)
	{
		auto ViewportKey{ LocalDisableVotes[i] };
		auto* GVP{ Cast<UGameViewportClient>(ViewportKey.ResolveObjectPtr()) };
		const auto* WorldContext{ GEngine->GetWorldContextFromGameViewport(GVP) };

		// Wrong context so ignore it, dead objects count as part of this context

		if (GVP && WorldContext)
		{
			if (!Context.ShouldApplyToWorldContext(*WorldContext))
			{
				continue;
			}
		}

		auto& VoteCount{ GlobalDisableVotes[ViewportKey] };

		if (VoteCount <= 1)
		{
			GlobalDisableVotes.Remove(ViewportKey);

			if (GVP && WorldContext)
			{
				GVP->SetForceDisableSplitscreen(false);
			}
		}
		else
		{
			--VoteCount;
		}

		LocalDisableVotes.RemoveAt(i);
	}
}

void UGameFeatureAction_SplitscreenConfig::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	if (bDisableSplitscreen)
	{
		if (auto GameInstance{ WorldContext.OwningGameInstance })
		{
			if (auto* VC{ GameInstance->GetGameViewportClient() })
			{
				FObjectKey ViewportKey(VC);

				LocalDisableVotes.Add(ViewportKey);

				auto& VoteCount{ GlobalDisableVotes.FindOrAdd(ViewportKey) };
				VoteCount++;
				if (VoteCount == 1)
				{
					VC->SetForceDisableSplitscreen(true);
				}
			}
		}
	}
}
