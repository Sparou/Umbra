// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "Interface/OutlineInterface.h"
#include "UmbraBaseCharacter.generated.h"

class UTagManager;
class UTraversalComponent;
class UCharacterTrajectoryComponent;
class UAbilitySystemComponent;
class UMotionWarpingComponent;
class UAttributeSet;
class UGameplayEffect;

UCLASS()
class UMBRA_API AUmbraBaseCharacter : public ACharacter, public ICombatInterface, public IOutlineInterface
{
	GENERATED_BODY()

public:
  
	AUmbraBaseCharacter();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	float GetMoveSpeed(const FGameplayTag& Stance, const FGameplayTag& Locomotion);

	TObjectPtr<UAbilitySystemComponent> GetAbilitySystemComponent() { return AbilitySystemComponent; }

	/** ICombatInterface implementation */
	virtual FWeaponSocketLocations GetWeaponSocketLocations_Implementation() const override;
	virtual UAnimMontage* GetRandomHitReactMontage_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
	virtual void Die() override;

	/** IOutline Interface **/
	virtual void EnableOutline_Implementation(int32 StencilValue) override;
	virtual void DisableOutline_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTagManager* GetTagManager();

	UFUNCTION(BlueprintCallable)
	void StartDissolve();
	
protected:
  
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
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

	UPROPERTY()
	TObjectPtr<UTagManager> TagManager;
	
	UPROPERTY(EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const;
	virtual void InitializeDefaultAttributes() const;
	
	virtual void InitAbilityActorInfo();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> StartingAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float StandWalkSpeed = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float StandRunSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float CrouchWalkSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float CrouchRunSpeed = 300.f;

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
