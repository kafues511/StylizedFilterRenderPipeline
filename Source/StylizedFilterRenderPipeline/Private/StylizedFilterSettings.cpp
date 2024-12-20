// Copyright 2024 kafues511. All Rights Reserved.

#include "StylizedFilterSettings.h"

FStylizedFilterSettings::FStylizedFilterSettings()
{
	// to set all bOverride_.. by default to false
	FMemory::Memzero(this, sizeof(FStylizedFilterSettings));

	Enabled = false;
	FilterType = EStylizedFilterType::SNN;
	Kernel = 7;
}
