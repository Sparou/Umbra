// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UmbraBaseGameplayAbility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(UmbraAbilitiesLog, Log, All);

/**
 * 
 */
UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnSpawn
};

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetInputTag() const { return InputTag;}
	

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Activation")
	EAbilityActivationPolicy ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation")
	FGameplayTag InputTag;
};
