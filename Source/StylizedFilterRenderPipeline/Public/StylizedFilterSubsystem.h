// Copyright 2024 kafues511. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "StylizedFilterSettings.h"
#include "StylizedFilterSubsystem.generated.h"

class FStylizedFilterViewExtension;

/**
 * 
 */
UCLASS()
class STYLIZEDFILTERRENDERPIPELINE_API UStylizedFilterSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	/**  */
	static UStylizedFilterSubsystem* GetCurrent(const UWorld* World);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Stylized Filter Settings")
	const FStylizedFilterSettings& GetStylizedFilterSettings() const;

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Stylized Filter Settings")
	void SetStylizedFilterSettings(const FStylizedFilterSettings NewValue);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Stylized Filter Settings")
	void OverrideStylizedFilterSettings(const FStylizedFilterSettings& NewValue);

public:
	/**  */
	const FStylizedFilterSettings GetStylizedFilterSettingsForLock() const;

private:
	/**  */
	mutable FCriticalSection CriticalSection;

	/**  */
	TSharedPtr<FStylizedFilterViewExtension, ESPMode::ThreadSafe> ViewExtension;

	/**  */
	FStylizedFilterSettings StylizedFilterSettings;
};
