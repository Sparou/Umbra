// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "EnemyChoicePriorityMaxValues.h"
#include "EnemyChoicePriorityWeights.h"
#include "Engine/DataAsset.h"
#include "DA_EnemyChoicePriority.generated.h"

UCLASS()
class UMBRA_API UDA_EnemyChoicePriority : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI target selection")
	FEnemyChoicePriorityWeights PriorityWeights;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI target selection")
	FEnemyChoicePriorityMaxValues PriorityMaxValues;
};
