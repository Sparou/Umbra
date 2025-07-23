// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "UmbraBaseTrapAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrapPlacedDelegate, AUmbraEffectActor*, SpawnedTrapActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrapPlacementCancelledDelegate, const FGameplayAbilityTargetDataHandle&, LastTargetData);

class UAbilityTask_WaitTargetData;
class AGameplayAbilityWorldReticle;
class AUmbraEffectActor;
/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraBaseTrapAbility : public UUmbraBaseGameplayAbility
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Trap Placement")
	FOnTrapPlacedDelegate OnTrapPlaced;

	UPROPERTY(BlueprintAssignable, Category = "Trap Placement")
	FOnTrapPlacementCancelledDelegate OnTrapPlacementCancelled;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trap")
	TSubclassOf<AUmbraEffectActor> TrapClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trap")
	float TrapHeight = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trap")
	float TrapRadius = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float Distance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
	TSubclassOf<AGameplayAbilityWorldReticle> ReticleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	bool bDebugTargeting = false;

	UFUNCTION(BlueprintCallable, Category = "Trap Placement")
	void TryToPlaceTrap();
	
	UFUNCTION()
	void ConfirmTrapPlacement(const FGameplayAbilityTargetDataHandle& Data);
	UFUNCTION()
	void CancelTrapPlacement(const FGameplayAbilityTargetDataHandle& Data);

private:

	TObjectPtr<UAbilityTask_WaitTargetData> WaitTargetDataTask = nullptr;
};
