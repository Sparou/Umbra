// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UmbraBaseCharacter.generated.h"

class UAbilitySystemComponent;
class UMotionWarpingComponent;
class UAttributeSet;

UCLASS()
class UMBRA_API AUmbraBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
  
	AUmbraBaseCharacter();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath();

protected:
  
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	FName WeaponTipSocketName;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

};
