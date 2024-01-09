// Copyright (C) 2024 owoDra

#pragma once

#include "GameFramework/HUD.h"

#include "GFCHUD.generated.h"


UCLASS()
class GFCORE_API AGFCHUD : public AHUD
{
	GENERATED_BODY()
public:
	AGFCHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void PreInitializeComponents() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
