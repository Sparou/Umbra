// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "UmbraPlayerState.generated.h"

class UVitalAttributeSet;
class UMovementAttributeSet;
class UStealthAttributeSet;
class UUmbraAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraPlayerState : public APlayerState, public IAbilitySystemInterface
{

	GENERATED_BODY()
public:

	AUmbraPlayerState();

protected:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; };
	UVitalAttributeSet* GetVitalAttributeSet() const { return VitalAttributeSet; };
	UMovementAttributeSet* GetMovementAttributeSet() const { return MovementAttributeSet; };
	UStealthAttributeSet* GetStealthAttributeSet() const { return StealthAttributeSet; };
	
	int16 GetPlayerLevel() const { return Level; };


private:
	
	TObjectPtr<UUmbraAbilitySystemComponent> AbilitySystemComponent;
	TObjectPtr<UVitalAttributeSet> VitalAttributeSet;
	TObjectPtr<UMovementAttributeSet> MovementAttributeSet;
	TObjectPtr<UStealthAttributeSet> StealthAttributeSet;

	UPROPERTY(VisibleAnywhere, Replicated)
	int16 Level = 1;
};
