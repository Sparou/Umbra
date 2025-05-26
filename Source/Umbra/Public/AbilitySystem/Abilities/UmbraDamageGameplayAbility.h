// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "UmbraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraDamageGameplayAbility : public UUmbraBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UFUNCTION(BlueprintCallable)
	void WeaponTrace(TArray<AActor*>& OutHitActors, const TArray<AActor*>& ActorsToIgnore, const FVector& TraceStart, const FVector& TraceEnd, float Radius, bool bDrawDebug = false);
};
