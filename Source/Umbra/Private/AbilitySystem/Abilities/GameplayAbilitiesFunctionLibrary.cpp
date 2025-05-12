// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/GameplayAbilitiesFunctionLibrary.h"

#include "UmbraGameplayTags.h"

FGameplayTag UGameplayAbilitiesFunctionLibrary::GetActorPositionRelativeToTarget(const AActor* Actor,
                                                                                 const AActor* Target, float HeightDifferenceThreshold)
{
	FUmbraGameplayTags UGT = FUmbraGameplayTags::Get();
  
	FVector ActorLocation = Actor->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	float HeightDifference = ActorLocation.Z - TargetLocation.Z;
	if (HeightDifference > HeightDifferenceThreshold)
	{
		return UGT.Position_Top;
	}
  
	FVector TargetForwardVector = Target->GetActorForwardVector();
	FVector TargetRightVector = Target->GetActorRightVector();
	FVector Direction = ActorLocation - TargetLocation;

	float Distance = Direction.Size();
	Direction.Normalize();

	// Убийца находится спереди или сзади?
	float ForwardDot = FVector::DotProduct(TargetForwardVector, Direction);
	float RightDot = FVector::DotProduct(TargetRightVector, Direction);
	UE_LOG(LogTemp, Warning, TEXT("ForwardDot: %f, RightDot: %f"), ForwardDot, RightDot);

	if (ForwardDot > 0.5f) // Угол < 60° — игрок находится спереди
	{
		return UGT.Position_Front;
	}
	if (ForwardDot < -0.5f) // Угол > 120° — игрок находится сзади
	{
		return UGT.Position_Behind;
	}

	// Убийца находится слева или справа?
	if (RightDot > 0.5f) // Угол < 60° — справа
	{
		return UGT.Position_Right;
	}
	if (RightDot < -0.5f) // Угол > 120° — слева
	{
		return UGT.Position_Left;
	}

	return UGT.Position_Behind;
}
