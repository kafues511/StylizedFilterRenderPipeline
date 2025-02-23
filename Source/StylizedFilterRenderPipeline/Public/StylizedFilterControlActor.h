// Copyright 2024 kafues511. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StylizedFilterSettings.h"
#include "StylizedFilterControlActor.generated.h"

UCLASS()
class STYLIZEDFILTERRENDERPIPELINE_API AStylizedFilterControlActor : public AActor
{
	GENERATED_BODY()
	
public:
	AStylizedFilterControlActor();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	//~ Begin AActor interface
#if WITH_EDITOR
	virtual void RerunConstructionScripts() override;
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
	//~ End AAcotr interface

public:
	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stylized Filter Settings")
	FStylizedFilterSettings StylizedFilterSettings;
};
