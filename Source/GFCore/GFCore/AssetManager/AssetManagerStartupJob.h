// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/StreamableManager.h"


/**
 * Delegate to handles reporting progress from streamable handles 
 */
DECLARE_DELEGATE_OneParam(FAssetManagerStartupJobSubstepProgress, float /*NewProgress*/);


/** 
 * Handles reporting progress from streamable handles 
 */
struct FAssetManagerStartupJob
{
public:
	FAssetManagerStartupJob(const FString& InJobName, const TFunction<void(const FAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)>& InJobFunc, float InJobWeight)
		: JobFunc(InJobFunc)
		, JobName(InJobName)
		, JobWeight(InJobWeight)
	{}

public:
	//
	// Delegate to handles reporting progress from streamable handles 
	//
	FAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;

	//
	// Asynchronous startup processing functions
	//
	TFunction<void(const FAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)> JobFunc;

	//
	// Name of startup job
	//
	FString JobName;

	//
	// Weight of this startup job
	//
	float JobWeight;

	//
	// Last updated time
	//
	mutable double LastUpdate{ 0 };
	
public:
	/** 
	 * Perform actual loading, will return a handle if it created one 
	 */
	TSharedPtr<FStreamableHandle> DoJob() const;

	/**
	 * Notify that the startup job steps have been updated.
	 */
	void UpdateSubstepProgress(float NewProgress) const { SubstepProgressDelegate.ExecuteIfBound(NewProgress); }

	void UpdateSubstepProgressFromStreamable(TSharedRef<FStreamableHandle> StreamableHandle) const
	{
		if (SubstepProgressDelegate.IsBound())
		{
			// StreamableHandle::GetProgress traverses() a large graph and is quite expensive

			auto Now{ FPlatformTime::Seconds() };
			if (LastUpdate - Now > 1.0 / 60)
			{
				SubstepProgressDelegate.Execute(StreamableHandle->GetProgress());
				LastUpdate = Now;
			}
		}
	}
};
