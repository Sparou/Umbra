// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraTraversalAbility.h"
#include "MantleAbility.generated.h"

struct FTraversalActionMontage;
/**
 * 
 */
UCLASS()
class UMBRA_API UMantleAbility : public UUmbraTraversalAbility
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable)
	bool FindTraversalActionMontage(const FHitResult& TopHitResult);

	
};
