// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "Interface/OutlineInterface.h"
#include "UmbraBaseCharacter.generated.h"

class UTraversalActionsData;
class UUmbraAbilitySystemComponent;
class UCombatData;
class UWeaponComponent;
class UMeshComponent;
class UMaterialInterface;
class UCharacterTrajectoryComponent;
class UMotionWarpingComponent;
class UGameplayEffect;
class UVitalAttributeSet;
class UMovementAttributeSet;
struct FGameplayAbilityActivationInfo;

UCLASS()
class UMBRA_API AUmbraBaseCharacter : public ACharacter, public ICombatInterface, public IOutlineInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AUmbraBaseCharacter();
	
protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath();

	/** ICombatInterface implementation */
	virtual FWeaponSocketLocations GetWeaponSocketLocations_Implementation() const override;
	virtual UAnimMontage* GetRandomHitReactMontage_Implementation(FGameplayAbilityActivationInfo AbilityActivationInfo, float SeedMultiplier = 100.f) override;
	virtual UAnimMontage* GetRandomMeleeAttackMontage_Implementation(FGameplayAbilityActivationInfo AbilityActivationInfo, float SeedMultiplier = 100.f) override;
	virtual FVector GetProjectileSpawnLocation_Implementation() const override;
	virtual void SetWarp_Implementation(FName WarpName, FVector TargetLocation, FRotator TargetRotation) override;
	virtual bool IsDead_Implementation() const override;
	virtual void Die() override;
	
	/** IOutline Interface **/
	virtual void EnableOutline_Implementation(int32 StencilValue) override;
	virtual void DisableOutline_Implementation() override;

	/** AbilitySystem Interface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; };
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UTraversalActionsData* GetTraversalActionsData() const { return TraversalActionsData; };
	UFUNCTION(BlueprintCallable, Category = "Ability System")
	UVitalAttributeSet* GetVitalAttributeSet() const { return VitalAttributeSet; }
	UFUNCTION(BlueprintCallable, Category = "Ability System")
	UMovementAttributeSet* GetMovementAttributeSet() const { return MovementAttributeSet; }

	UFUNCTION(BlueprintCallable)
	void StartDissolve();
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<UWeaponComponent> WeaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UCombatData> CombatData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UTraversalActionsData> TraversalActionsData;
	
	TObjectPtr<UUmbraAbilitySystemComponent> AbilitySystemComponent;
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const;
	
	virtual void ApplyStartingEffects();
	virtual void InitAbilityActorInfo();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability System")
	TArray<TSubclassOf<class UGameplayAbility>> StartingAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability System")
	TArray<TSubclassOf<class UGameplayEffect>> StartingEffects;

	TObjectPtr<UVitalAttributeSet> VitalAttributeSet;
	TObjectPtr<UMovementAttributeSet> MovementAttributeSet;

	void AddCharacterAbilities();
	
	/* Dissolve Effects */

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterial;

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterial);

	UFUNCTION()
	void Dissolve();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDissolve();
	
private:
	bool bIsDead = false;
};
