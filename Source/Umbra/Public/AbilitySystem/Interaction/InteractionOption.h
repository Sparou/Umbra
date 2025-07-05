#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "InteractionOption.generated.h"

USTRUCT(BlueprintType)
struct FInteractionOption
{
	GENERATED_BODY()
	
	FString Tooltip = FString("Interact");
	
	TSubclassOf<UGameplayAbility> InteractionAbility;
};