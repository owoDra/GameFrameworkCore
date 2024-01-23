// Copyright (C) 2024 owoDra

#pragma once

#include "WorldOption.generated.h"


/**
 * Base class for extra world setting option
 */
UCLASS(Abstract, Const, DefaultToInstanced, EditInlineNew)
class GFCORE_API UWorldOption : public UObject
{
	GENERATED_BODY()
public:
	UWorldOption(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};
