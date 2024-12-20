// Copyright 2024 kafues511. All Rights Reserved.

#include "StylizedFilterSubsystem.h"
#include "StylizedFilterViewExtension.h"

void UStylizedFilterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// Initializing Scene view extension responsible for rendering regions.
	ViewExtension = FSceneViewExtensions::NewExtension<FStylizedFilterViewExtension>(this);
}

void UStylizedFilterSubsystem::Deinitialize()
{
	ENQUEUE_RENDER_COMMAND(StylizedFilterSubsystem_Deinitialize)([this](FRHICommandListImmediate& RHICmdList)
	{
		ViewExtension->Invalidate();
		ViewExtension.Reset();
		ViewExtension = nullptr;
	});
}

UStylizedFilterSubsystem* UStylizedFilterSubsystem::GetCurrent(const UWorld* World)
{
	return UWorld::GetSubsystem<UStylizedFilterSubsystem>(World);
}

const FStylizedFilterSettings& UStylizedFilterSubsystem::GetStylizedFilterSettings() const
{
	return StylizedFilterSettings;
}

void UStylizedFilterSubsystem::SetStylizedFilterSettings(FStylizedFilterSettings NewValue)
{
	StylizedFilterSettings = NewValue;
}

#define SET_PP(NAME) \
if (NewValue.bOverride_##NAME) \
{ \
	Value.NAME = NewValue.NAME; \
} \

void UStylizedFilterSubsystem::OverrideStylizedFilterSettings(const FStylizedFilterSettings& NewValue)
{
	FStylizedFilterSettings& Value = StylizedFilterSettings;

	SET_PP(Enabled);
	SET_PP(FilterType);
	SET_PP(Kernel);
}

const FStylizedFilterSettings UStylizedFilterSubsystem::GetStylizedFilterSettingsForLock() const
{
	FScopeLock Lock(&CriticalSection);
	return StylizedFilterSettings;
}
