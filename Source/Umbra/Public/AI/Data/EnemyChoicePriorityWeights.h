// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "EnemyChoicePriorityWeights.generated.h"

USTRUCT(BlueprintType)
struct UMBRA_API FEnemyChoicePriorityWeights
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI target selection")
	float ThreatWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI target selection")
	float DistanceWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI target selection")
	float TimeSinceSeenWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI target selection")
	float IsCurrentTargetBonus = 1.f;
};