// Copyright 2024 kafues511. All Rights Reserved.

#include "StylizedFilterControlActor.h"
#include "StylizedFilterSubsystem.h"

AStylizedFilterControlActor::AStylizedFilterControlActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AStylizedFilterControlActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (UStylizedFilterSubsystem* Subsystem = UStylizedFilterSubsystem::GetCurrent(GetWorld()))
	{
		Subsystem->OverrideStylizedFilterSettings(StylizedFilterSettings);
	}
}

void AStylizedFilterControlActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if WITH_EDITOR
void AStylizedFilterControlActor::RerunConstructionScripts()
{
	Super::RerunConstructionScripts();
}

void AStylizedFilterControlActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (UStylizedFilterSubsystem* Subsystem = UStylizedFilterSubsystem::GetCurrent(GetWorld()))
	{
		Subsystem->OverrideStylizedFilterSettings(StylizedFilterSettings);
	}
}
#endif