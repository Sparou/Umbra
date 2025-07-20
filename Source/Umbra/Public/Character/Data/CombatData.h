// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CombatData.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UCombatData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> MeleeAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> DistantAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> HitReactMontages;
	
};
