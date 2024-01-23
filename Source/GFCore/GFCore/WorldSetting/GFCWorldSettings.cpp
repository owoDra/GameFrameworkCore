// Copyright (C) 2024 owoDra

#include "GFCWorldSettings.h"

#include "WorldSetting/WorldOption.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFCWorldSettings)


AGFCWorldSettings::AGFCWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


const UWorldOption* AGFCWorldSettings::GetOptionByClass(TSubclassOf<UWorldOption> InClass) const
{
	for (const auto& Option : WorldOptions)
	{
		if (Option->IsA(InClass))
		{
			return Option;
		}
	}

	return nullptr;
}
