// Copyright 2024 kafues511. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StylizedFilterSettings.generated.h"

/** フィルタの設定 */
USTRUCT(BlueprintType, meta = (HiddenByDefault))
struct FStylizedFilterSettings
{
	GENERATED_USTRUCT_BODY()

	// first all bOverride_... as they get grouped together into bitfields

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_Enabled : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_Kernel : 1;

	// -----------------------------------------------------------------------

	/** フィルタの有効性 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylized Filter Settings", meta = (editcondition = "bOverride_Enabled", DisplayName = "Enabled"))
	uint32 Enabled : 1;

	/** カーネル */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylized Filter Settings", meta = (editcondition = "bOverride_Kernel", DisplayName = "Kernel"))
	int32 Kernel;

	// -----------------------------------------------------------------------

	STYLIZEDFILTERRENDERPIPELINE_API FStylizedFilterSettings();
};
