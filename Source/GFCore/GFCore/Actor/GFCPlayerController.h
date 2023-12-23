// Copyright (C) 2023 owoDra

#pragma once

#include "GameFramework/PlayerController.h"

#include "GFCPlayerController.generated.h"


/** 
 * Minimal class that supports extension by game feature plugins 
 */
UCLASS(Blueprintable)
class GFCORE_API AGFCPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AGFCPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void ReceivedPlayer() override;
	virtual void SetPawn(APawn* InPawn) override;
	virtual void OnPossess(class APawn* APawn) override;
	virtual void OnUnPossess() override;
	virtual void PlayerTick(float DeltaTime) override;

public:
	/**
	 * Delegate to notify that PostProcessInput has been executed in PlayerController
	 */
	DECLARE_EVENT_TwoParams(AGFCPlayerController, FPostProcessInputDelegate, const float /*DeltaTime*/, const bool /*bGamePaused*/);
	FPostProcessInputDelegate OnPostProcessInput;

protected:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

};
