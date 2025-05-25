// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "CarryBodyAbility.generated.h"

class AUmbraBaseCharacter;
class AUmbraPlayerCharacter;
/**
 * 
 */
UCLASS()
class UMBRA_API UCarryBodyAbility : public UUmbraBaseGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	float ActivationDistance;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* SourcePickupMontage;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* TargetPickupMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* SourceReleaseMontage;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* TargetReleaseMontage;
	

protected:

private:

	TObjectPtr<AUmbraPlayerCharacter> SourceCharacter;
	TObjectPtr<AUmbraBaseCharacter> TargetCharacter;

	bool InitializeSourceCharacter();
	bool InitializeTargetCharacter();
	bool ValidateActivationDistance();

	void PibckupBody();
	void ReleaseBody();

	UFUNCTION()
	void OnMontageBlendingOut();
};
