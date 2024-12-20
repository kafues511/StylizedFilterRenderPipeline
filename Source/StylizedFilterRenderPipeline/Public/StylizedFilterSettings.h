// Copyright 2024 kafues511. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StylizedFilterSettings.generated.h"

UENUM(BlueprintType, meta = (HiddenByDefault))
enum class EStylizedFilterType : uint8
{
	/** SNN (Symmetric Nearest Neighbor) */
	SNN			UMETA(DisplayName = "SNN"),

	/** Kuwahara (Kuwahara Filter) */
	Kuwahara	UMETA(DisplayName = "Kuwahara"),

	Max			UMETA(Hidden),
};

/** フィルタの設定 */
USTRUCT(BlueprintType, meta = (HiddenByDefault))
struct FStylizedFilterSettings
{
	GENERATED_USTRUCT_BODY()

	// first all bOverride_... as they get grouped together into bitfields

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_Enabled : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_FilterType : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_Kernel : 1;

	// -----------------------------------------------------------------------

	/** フィルタの有効性 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylized Filter Settings", meta = (editcondition = "bOverride_Enabled", DisplayName = "Enabled"))
	uint32 Enabled : 1;

	/** フィルタの種類 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylized Filter Settings", meta = (editcondition = "bOverride_FilterType", DisplayName = "Filter Type"))
	EStylizedFilterType FilterType;

	/** カーネル */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylized Filter Settings", meta = (editcondition = "bOverride_Kernel", DisplayName = "Kernel"))
	int32 Kernel;

	// -----------------------------------------------------------------------

	STYLIZEDFILTERRENDERPIPELINE_API FStylizedFilterSettings();

	/**
	 * 有効性を取得
	 * @return bool 有効な場合はTrueを返します。
	 */
	bool IsValid() const
	{
		return Enabled
			&& (FilterType == EStylizedFilterType::SNN || FilterType == EStylizedFilterType::Kuwahara)
			&& Kernel >= 2;  // 繰り上げでKernel(3)として扱われるので許容
	}
};
