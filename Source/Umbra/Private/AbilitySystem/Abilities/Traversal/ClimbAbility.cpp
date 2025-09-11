// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Traversal/ClimbAbility.h"

#include "AbilitySystemComponent.h"
#include "UmbraGameplayTags.h"
#include "AbilitySystem/GameplayEffect/UmbraGameplayEffectContext.h"
#include "Character/UmbraBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Player/UmbraPlayerController.h"

bool UClimbAbility::ValidateInitialClimbSurface()
{
	if (!FindObstacleHitResult(ObstacleHitResult, ObstacleDetectionDownOffset, ObstacleDetectionIterations, ObstacleDetectionDistance, ObstacleDetectionOffsetStep, ObstacleDetectionDebug))
	{
		return false;
	}

	if (!FindObstacleEdgeResult(EdgeHitResult, ObstacleHitResult, EdgeDetectionIterations, EdgeDetectionOffsetStep, EdgeDetectionDistance, EdgeDetectionThreshold, EdgeDetectionDebug))
	{
		return false;
	}

	if (!FindObstacleTopResult(TopHitResult, EdgeHitResult, TopDetectionIterations, TopDetectionTraceHeight, TopDetectionDebug, TopDetectionDebug))
	{
		return false;
	}

	float CapsuleHalfHeight = 0;
	float CapsuleHalfRadius = 0;

	if (UCapsuleComponent* CapsuleComponent = GetCapsuleComponent())
	{
		CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
		CapsuleHalfRadius = CapsuleComponent->GetScaledCapsuleRadius() / 2.f;
	}

	const FVector OverlapLocation = TopHitResult.ImpactPoint + GetAvatarActorFromActorInfo()->GetActorForwardVector() * SurfaceValidationXOffset + FVector::UpVector * SurfaceValidationZOffset;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	
	if (!HasEnoughSpace(CapsuleHalfHeight, CapsuleHalfRadius, OverlapLocation, ActorsToIgnore, SurfaceValidationDebug))
	{
		return false;
	}
	
	return true;
}

bool UClimbAbility::FindTraversalActionMontage()
{
	const float ObstacleHeight = TopHitResult.ImpactPoint.Z - GetUmbraCharacter()->GetMesh()->GetSocketLocation("root").Z;;

	UE_LOG(UmbraAbilitiesLog, Log, TEXT("Climb-able obstacle height = [%f]"), ObstacleHeight);

	TMap<FGameplayTag, FTraversalAction> SuitableActionsMap;
	if (!GetTraversalActions()->FindActionsByTag(SuitableActionsMap, FUmbraGameplayTags::Get().Traversal_Climb))
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("There are not valid climb actions in traversal actions data"));
		return false;
	}

	for (const auto& Pair : SuitableActionsMap)
	{
		float MinRange = Pair.Value.ObstacleHeightRange.X;
		float MaxRange = Pair.Value.ObstacleHeightRange.Y;
    
		if (!FMath::IsWithinInclusive(ObstacleHeight, MinRange, MaxRange))
		{
			continue;
		}
    
		if (GetTraversalActions()->GetRandomTraversalActionFromAbility(TraversalActionMontage, Pair.Key, GetCurrentActivationInfo()))
		{
			UE_LOG(UmbraAbilitiesLog, Log, TEXT("Traversal Montage was set in [%s]"), *GetNameSafe(this));
			TraversalActionTag = Pair.Key;
			return true;
		}
	}

	return false;
}

FActiveGameplayEffectHandle UClimbAbility::ApplyClimbEffectToOwner()
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ClimbEffectClass);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(UmbraAbilitiesLog, Error, TEXT("Failed to apply climb effect in [%s]"), *GetNameSafe(this));
		return FActiveGameplayEffectHandle(); 
	}
	
	FGameplayEffectContext* BaseContext = SpecHandle.Data.Get()->GetContext().Get();
	FUmbraGameplayEffectContext* UmbraContext = static_cast<FUmbraGameplayEffectContext*>(BaseContext);

	if (!UmbraContext)
	{
		UE_LOG(UmbraAbilitiesLog, Error, TEXT("Failed to apply climb effect in [%s] 2"), *GetNameSafe(this));
		return FActiveGameplayEffectHandle();
	}
	UE_LOG(LogTemp, Warning, TEXT("Top Hit Result = [%s]"), *TopHitResult.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Server: UmbraContext pointer is: %p"), UmbraContext);
	UmbraContext->SetTopHitResult(TopHitResult);
	return GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

AUmbraPlayerController* UClimbAbility::GetUmbraPlayerController()
{
	if (!UmbraPlayerController.IsValid())
	{
		return nullptr;
	}

	return UmbraPlayerController.Get();
}

void UClimbAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (!GetUmbraCharacter())
	{
		return;
	}

	UmbraPlayerController = Cast<AUmbraPlayerController>(GetUmbraCharacter()->GetController());

	if (!UmbraPlayerController.IsValid())
	{
		UE_LOG(UmbraAbilitiesLog, Error, TEXT("Umbra Player Controller is null in [%s]"), *GetNameSafe(this));
	}
}
