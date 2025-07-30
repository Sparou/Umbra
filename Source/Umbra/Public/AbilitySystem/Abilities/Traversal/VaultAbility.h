// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraTraversalAbility.h"
#include "VaultAbility.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UVaultAbility : public UUmbraTraversalAbility
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable)
	bool FindTraversalActionMontage(const FHitResult& TopHitResult, const FHitResult& DepthHitResult, const FHitResult& VaultHitResult);
	
};
