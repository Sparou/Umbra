// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraDamageGameplayAbility.h"
#include "UmbraProjectileGameplayAbility.generated.h"

class AUmbraBaseProjectile;
/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraProjectileGameplayAbility : public UUmbraDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AUmbraBaseProjectile> ProjectileClass;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector& SpawnLocation, bool bDrawDebug = false);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ToolTip = "Blackboard value name for case of using the ability by AI"))
	FName BlackboardVariableName = "CurrentEnemy";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ToolTip = "Name of a bone used for aiming"))
	FName BoneName = "spine_03";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", meta = (ToolTip = "Lenght of a trace used to find the point the player is looking at"))
	float TraceDistance = 10000.f;
	
	UFUNCTION(BlueprintCallable)
	FVector GetTargetLocation();
};
