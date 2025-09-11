 // Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UmbraAbilitySystemComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(UmbraAbilitySystemLog, Log, All);

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	
	//~ Input
	void AbilityInputTagPressed(const FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void ProcessAbilityInput(float DeltaTime, bool bGamePause = false);

	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedHandles;
	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedHandles;
	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldHandles;
	
	
};
