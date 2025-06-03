// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NiagaraSystem.h"
#include "CombatInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FWeaponSocketLocations
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sockets")
	FVector WeaponBase = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sockets")
	FVector WeaponTip = FVector::ZeroVector;
};

/**
 * 
 */
class UMBRA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Die() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FWeaponSocketLocations GetWeaponSocketLocations() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetProjectileSpawnLocation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetWarp(FName WarpName, FVector TargetLocation, FRotator TargetRotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetRandomHitReactMontage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetRandomMeleeAttackMontage();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;
};
