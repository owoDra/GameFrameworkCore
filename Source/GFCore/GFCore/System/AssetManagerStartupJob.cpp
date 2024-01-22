// Copyright (C) 2024 owoDra

#include "AssetManagerStartupJob.h"

#include "GFCoreLogs.h"


TSharedPtr<FStreamableHandle> FAssetManagerStartupJob::DoJob() const
{
	UE_LOG(LogGameCore_Startup, Log, TEXT("Startup job \"%s\" starting"), *JobName);

	const auto JobStartTime{ FPlatformTime::Seconds() };

	TSharedPtr<FStreamableHandle> Handle;
	JobFunc(*this, Handle);

	if (Handle.IsValid())
	{
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate::CreateRaw(this, &FAssetManagerStartupJob::UpdateSubstepProgressFromStreamable));
		Handle->WaitUntilComplete(0.0f, false);
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate());
	}

	UE_LOG(LogGameCore_Startup, Log, TEXT("Startup job \"%s\" took %.2f seconds to complete"), *JobName, FPlatformTime::Seconds() - JobStartTime);

	return Handle;
}
