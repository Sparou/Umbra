#pragma once

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
struct FStealthKillMontages
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth Kill")
	UAnimMontage* KillerMontage = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth Kill")
	UAnimMontage* VictimMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth Kill")
	FVector KillerStarterLocation;
};

