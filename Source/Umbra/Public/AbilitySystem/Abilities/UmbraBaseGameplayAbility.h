// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UmbraBaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	FGameplayTag StartupInputTag;
};
