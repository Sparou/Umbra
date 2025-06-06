﻿#pragma once

#include "UmbraCoreTypes.generated.h"

UENUM(BlueprintType)
enum class EStealthKillPosition : uint8
{
	Behind UMETA(DisplayName = "Behind"),
	Front UMETA(DisplayName = "Front"),
	Top UMETA(DisplayName = "Top"),
	Ledge UMETA(DisplayName = "Ledge"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

USTRUCT(BlueprintType)
struct FMotionWarpingParams
{
	GENERATED_BODY()

	FVector Location = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;
};
