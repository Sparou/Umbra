#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "InteractionOption.generated.h"

USTRUCT(BlueprintType)
struct FInteractionOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Tooltip = FString("Interact");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> InteractionAbility;
};