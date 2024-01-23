// Copyright (C) 2024 owoDra

#pragma once

#include "GameFramework/WorldSettings.h"

#include "GFCWorldSettings.generated.h"

class UWorldOption;


/**
 * WorldSetting class for adding extra options
 */
UCLASS()
class GFCORE_API AGFCWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	AGFCWorldSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditAnywhere, Instanced, Category = "World Options")
	TArray<TObjectPtr<const UWorldOption>> WorldOptions;

public:
	const UWorldOption* GetOptionByClass(TSubclassOf<UWorldOption> InClass) const;

	template<typename T>
	const T* GetOption() const
	{
		return Cast<T>(GetOptionByClass(T::StaticClass()));
	}

	template<typename T>
	static const T* GetOptionFromWorld(const UWorld* InWorld)
	{
		if (auto* ThisWorldSetting{ Cast<AGFCWorldSettings>(InWorld->GetWorldSettings()) })
		{
			return ThisWorldSetting->GetOption<T>();
		}

		return nullptr;
	}

};
