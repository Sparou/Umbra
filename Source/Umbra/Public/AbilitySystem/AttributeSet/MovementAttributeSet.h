// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AttributeSet/UmbraAttributeSet.h"
#include "MovementAttributeSet.generated.h"

/**
 * Набор аттрибутов, регулирующих движение персонажей.
 */
UCLASS()
class UMBRA_API UMovementAttributeSet : public UUmbraAttributeSet
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StandWalkSpeed, Category = "Stand")
	FGameplayAttributeData StandWalkSpeed;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, StandWalkSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StandRunSpeed, Category = "Stand")
	FGameplayAttributeData StandRunSpeed;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, StandRunSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CrouchWalkSpeed, Category = "Crouch")
	FGameplayAttributeData CrouchWalkSpeed;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, CrouchWalkSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CrouchRunSpeed, Category = "Crouch")
	FGameplayAttributeData CrouchRunSpeed;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, CrouchRunSpeed);
	
protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:

	UFUNCTION()
	void OnRep_StandWalkSpeed(const FGameplayAttributeData& OldStandWalkSpeed) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, StandWalkSpeed, OldStandWalkSpeed); }
	UFUNCTION()
	void OnRep_StandRunSpeed(const FGameplayAttributeData& OldStandRunSpeed) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, StandRunSpeed, OldStandRunSpeed); }
	UFUNCTION()
	void OnRep_CrouchWalkSpeed(const FGameplayAttributeData& OldCrouchWalkSpeed) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, CrouchWalkSpeed, OldCrouchWalkSpeed); }
	UFUNCTION()
	void OnRep_CrouchRunSpeed(const FGameplayAttributeData& OldCrouchRunSpeed) const { GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, CrouchRunSpeed, OldCrouchRunSpeed); }
};
