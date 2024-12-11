// Copyright 2024 kafues511. All Rights Reserved.

#include "StylizedFilterRenderPipelineModule.h"
#include "ShaderCore.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FStylizedFilterRenderPipelineModule"

void FStylizedFilterRenderPipelineModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("StylizedFilterRenderPipeline"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/StylizedFilterRenderPipeline"), PluginShaderDir);
}

void FStylizedFilterRenderPipelineModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStylizedFilterRenderPipelineModule, StylizedFilterRenderPipeline)
