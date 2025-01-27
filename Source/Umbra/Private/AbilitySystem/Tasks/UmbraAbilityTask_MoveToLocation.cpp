// Copyrighted by Vorona Games

#include "AbilitySystem/Tasks/UmbraAbilityTask_MoveToLocation.h"

UUmbraAbilityTask_MoveToLocation* UUmbraAbilityTask_MoveToLocation::CreateMoveToLocation(
	UGameplayAbility* OwningAbility, FVector TargetLocation, float Duration)
{
	UUmbraAbilityTask_MoveToLocation* MyObj = NewAbilityTask<UUmbraAbilityTask_MoveToLocation>(OwningAbility);
	MyObj->TargetLocation = TargetLocation;
	MyObj->DurationOfMovement = Duration;
	return MyObj;
}
