// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "UmbraBaseCharacter.generated.h"

class UCharacterTrajectoryComponent;
class UAbilitySystemComponent;
class UMotionWarpingComponent;
class UAttributeSet;
class UGameplayEffect;

UCLASS()
class UMBRA_API AUmbraBaseCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
  
	AUmbraBaseCharacter();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float BaseWalkSpeed = 300.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float BaseRunSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float BaseCrouchSpeed = 300.f;


	/* For invisibility */
	UPROPERTY(ReplicatedUsing = OnRep_InvisibilityChanged)
	bool bIsInvisible;

	UPROPERTY(EditAnywhere, Category = "Invisibility")
	UMaterialInterface* InvisibleMaterial;
	
	UPROPERTY(EditAnywhere, Category = "Invisibility")
	UMaterialInterface* OriginalMaterial;

	UPROPERTY(EditAnywhere, Category = "Invisibility")
	UMaterialInterface* InvisibleWeaponMaterials;
	
	UPROPERTY(EditAnywhere, Category = "Invisibility")
	UMaterialInterface* OriginalWeaponMaterials;
	
	UFUNCTION()
	void OnRep_InvisibilityChanged();

	UFUNCTION()
	void SetInvisibility(bool bInvisible);

	UFUNCTION(Server, Reliable)
	void ServerSetInvisibility(bool bInvisible);
	

	/** ICombatInterface implementation */
	virtual FWeaponSocketLocations GetWeaponSocketLocations_Implementation() const override;
	virtual UAnimMontage* GetRandomHitReactMontage_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
	virtual void Die() override;
	
protected:
  
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	FName WeaponBaseSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<UAnimMontage*> HitReactMontages;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const;
	virtual void InitializeDefaultAttributes() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UCharacterTrajectoryComponent> TrajectoryComponent;

	virtual void InitAbilityActorInfo();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> StartingAbilities;

private:

	bool bIsDead = false;
};
