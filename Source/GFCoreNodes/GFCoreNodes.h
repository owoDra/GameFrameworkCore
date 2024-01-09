// Copyright (C) 2024 owoDra

#pragma once

#include "Modules/ModuleManager.h"

/**
 *  Modules for the blueprint node features of the GameFrameworkCore plugin
 */
class FGFCoreNodesModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
