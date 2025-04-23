// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FEmotionReactionRow.generated.h"

USTRUCT(BlueprintType)
struct UMBRA_API FEmotionReactionRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FearDelta = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BelligerenceDelta = 0.0f;
};