// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "UmbraInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FUmbraInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FUmbraInputAction> AbilitiesInputActions;

	const FUmbraInputAction* GetInputActionByTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;
};
