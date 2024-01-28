// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/AssetManager.h"

#include "AssetManagerStartupJob.h"

#include "GFCAssetManager.generated.h"


/**
 *	Game implementation of the asset manager that overrides functionality and stores game-specific types.
 *	It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic.
 *	This class is used by setting 'AssetManagerClassName' in DefaultEngine.ini.
 */
UCLASS(Config = Game)
class GFCORE_API UGFCAssetManager : public UAssetManager
{
	GENERATED_BODY()
protected:
	virtual void StartInitialLoading() override;

#if WITH_EDITOR
	virtual void PreBeginPIE(bool bStartSimulate) override;
#endif // WITH_EDITOR


protected:
	//
	// The list of tasks to execute on startup. Used to track startup progress.
	//
	TArray<FAssetManagerStartupJob> StartupJobs;

	//
	// Assets loaded and tracked by the asset manager.
	//
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	//
	// Used for a scope lock when modifying the list of load assets.
	//
	FCriticalSection LoadedAssetsCritical;

public:
	/**
	 * Returns the AssetManager singleton object.
	 */
	static UGFCAssetManager& Get();

	/**
	 * Logs all assets currently loaded and tracked by the asset manager.
	 */
	static void DumpLoadedAssets();

	/**
	 * Returns the asset referenced by a TSoftObjectPtr.  This will synchronously load the asset if it's not already loaded.
	 */
	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	/**
	 * Returns the asset referenced by a FPrimaryAssetId.  This will synchronously load the asset if it's not already loaded.
	 */
	template<typename AssetType>
	static AssetType* GetAsset(const FPrimaryAssetId& AssetId, bool bKeepInMemory = true);

	/**
	 * Returns the subclass referenced by a TSoftClassPtr.  This will synchronously load the asset if it's not already loaded.
	 */
	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);


protected:
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
	static bool ShouldLogAssetLoads();

	/**
	 * Thread safe way of adding a loaded asset to keep in memory.
	 */
	void AddLoadedAsset(const UObject* Asset);

	/**
	 * Flushes the StartupJobs array. Processes all startup work.
	 */
	void DoAllStartupJobs();

	/**
	 * Called periodically during loads, could be used to feed the status to a loading screen
	 */
	void UpdateInitialGameContentLoadPercent(float GameContentPercent);
};


template<typename AssetType>
AssetType* UGFCAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	const auto& AssetPath{ AssetPointer.ToSoftObjectPath() };

	if (AssetPath.IsValid())
	{
		auto* LoadedAsset{ AssetPointer.Get() };

		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));

			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		// Added to loaded asset list.

		if (LoadedAsset && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}

		return LoadedAsset;
	}

	return nullptr;
}

template<typename AssetType>
inline AssetType* UGFCAssetManager::GetAsset(const FPrimaryAssetId& AssetId, bool bKeepInMemory)
{
	auto& AssetManager{ Get() };
	auto AssetPath{ AssetManager.GetPrimaryAssetPath(AssetId) };

	if (AssetPath.IsValid())
	{
		auto* LoadedAsset{ Cast<AssetType>(AssetPath.ResolveObject()) };

		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));

			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetId.ToString());
		}
		
		// Added to loaded asset list.

		if (LoadedAsset && bKeepInMemory)
		{
			AssetManager.AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}

		return LoadedAsset;
	}

	return nullptr;
}

template<typename AssetType>
TSubclassOf<AssetType> UGFCAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	const auto& AssetPath{ AssetPointer.ToSoftObjectPath() };

	if (AssetPath.IsValid())
	{
		auto LoadedSubclass{ AssetPointer.Get() };

		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));

			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		// Added to loaded asset list.

		if (LoadedSubclass && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}

		return LoadedSubclass;
	}

	return nullptr;
}
