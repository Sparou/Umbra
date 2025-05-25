// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "EnemyChoicePriorityMaxValues.generated.h"

USTRUCT(BlueprintType)
struct UMBRA_API FEnemyChoicePriorityMaxValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI target selection")
	float MaxThreatLevel = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI target selection")
	float MaxDistance = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI target selection")
	float MaxTimeNotSeen = 60.f;
};