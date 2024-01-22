// Copyright (C) 2024 owoDra

#include "GFCAssetManager.h"

#include "GFCoreLogs.h"

#include "Misc/ScopedSlowTask.h"

//#include "LyraLogChannels.h"
//#include "LyraGameplayTags.h"
//#include "LyraGameData.h"
//#include "AbilitySystemGlobals.h"
//#include "Character/LyraPawnData.h"
//#include "Misc/App.h"
//#include "Stats/StatsMisc.h"
//#include "Engine/Engine.h"
//#include "AbilitySystem/LyraGameplayCueManager.h"
//
//#include "System/GFCAssetManagerStartupJob.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCAssetManager)

//////////////////////////////////////////////////////////////////////

static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("GameCore.Asset.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(UGFCAssetManager::DumpLoadedAssets)
);

//////////////////////////////////////////////////////////////////////

#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FAssetManagerStartupJob(#JobFunc, [this](const FAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)

//////////////////////////////////////////////////////////////////////

void UGFCAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UGFCAssetManager::StartInitialLoading");

	// This does all of the scanning, need to do this now even if loads are deferred

	Super::StartInitialLoading();

	// Run all the queued up startup jobs

	DoAllStartupJobs();
}

#if WITH_EDITOR
void UGFCAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);

	{
		FScopedSlowTask SlowTask(0, NSLOCTEXT("Editor", "BeginLoadingPIEData", "Loading PIE Data"));

		const auto bShowCancelButton{ false };
		const auto bAllowInPIE{ true };

		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);

		// Intentionally after GetGameData to avoid counting GameData time in this timer

		SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);

		// You could add preloading of anything else needed for the experience we'll be using here
		// (e.g., by grabbing the default experience from the world settings + the experience override in developer settings)
	}
}
#endif


UGFCAssetManager& UGFCAssetManager::Get()
{
	check(GEngine);

	if (auto* Manager{ Cast<UGFCAssetManager>(GEngine->AssetManager) })
	{
		return *Manager;
	}

	UE_LOG(LogGameCore_Asset, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini. It must be set to GFCAssetManager or or a class that inherits from GFCAssetManager!"));

	// Fatal error above prevents this from being called.

	return *NewObject<UGFCAssetManager>();
}

void UGFCAssetManager::DumpLoadedAssets()
{
	UE_LOG(LogGameCore_Asset, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const auto LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(LogGameCore_Asset, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(LogGameCore_Asset, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(LogGameCore_Asset, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}


UObject* UGFCAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.

		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool UGFCAssetManager::ShouldLogAssetLoads()
{
	static auto bLogAssetLoads{ FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads")) };

	return bLogAssetLoads;
}

void UGFCAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}


void UGFCAssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("UGFCAssetManager::DoAllStartupJobs");
	const auto AllStartupJobsStartTime{ FPlatformTime::Seconds() };

	if (IsRunningDedicatedServer())
	{
		// No need for periodic progress updates, just run the jobs

		for (const auto& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{
		if (StartupJobs.Num() > 0)
		{
			auto TotalJobValue{ 0.0f };
			for (const auto& StartupJob : StartupJobs)
			{
				TotalJobValue += StartupJob.JobWeight;
			}

			auto AccumulatedJobValue{ 0.0f };
			for (auto& StartupJob : StartupJobs)
			{
				const auto JobValue{ StartupJob.JobWeight };

				StartupJob.SubstepProgressDelegate.BindLambda(
					[This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
					{
						const auto SubstepAdjustment{ FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue };
						const auto OverallPercentWithSubstep{ (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue };

						This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
					}
				);

				StartupJob.DoJob();

				StartupJob.SubstepProgressDelegate.Unbind();

				AccumulatedJobValue += JobValue;

				UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
			}
		}
		else
		{
			UpdateInitialGameContentLoadPercent(1.0f);
		}
	}

	StartupJobs.Empty();

	UE_LOG(LogGameCore_Startup, Display, TEXT("All startup jobs took %.2f seconds to complete"), FPlatformTime::Seconds() - AllStartupJobsStartTime);
}

void UGFCAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{
	// Could route this to the early startup loading screen
}
