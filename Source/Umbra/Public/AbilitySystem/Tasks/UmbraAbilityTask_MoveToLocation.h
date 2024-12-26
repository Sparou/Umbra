// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "UmbraAbilityTask_MoveToLocation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUmbraMoveToLocationDelegate);

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraAbilityTask_MoveToLocation : public UAbilityTask
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UUmbraAbilityTask_MoveToLocation* CreateMoveToLocation(UGameplayAbility* OwningAbility, FVector TargetLocation, float Duration);
	
	UPROPERTY(BlueprintAssignable)
	FUmbraMoveToLocationDelegate OnTargetLocationReached;
	
private:

	FVector TargetLocation;
	float DurationOfMovement;
	
};
