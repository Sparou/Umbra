// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "UmbraActiveGameplayAbility.generated.h"

/**
 * This ability class is used for active abilities that are triggered by the player.
 */
UCLASS()
class UMBRA_API UUmbraActiveGameplayAbility : public UUmbraBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	FGameplayTag StartupInputTag;
};
