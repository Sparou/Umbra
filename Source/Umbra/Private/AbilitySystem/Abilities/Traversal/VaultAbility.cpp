// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Traversal/VaultAbility.h"
#include "UmbraGameplayTags.h"
#include "Character/UmbraBaseCharacter.h"

bool UVaultAbility::FindTraversalActionMontage(const FHitResult& TopHitResult, const FHitResult& DepthHitResult, const FHitResult& VaultHitResult)
{
	const float ObstacleHeight = TopHitResult.ImpactPoint.Z - GetUmbraCharacter()->GetMesh()->GetSocketLocation("root").Z;
	const float ObstacleDepth = TopHitResult.bBlockingHit && DepthHitResult.bBlockingHit ? FVector::Dist(TopHitResult.ImpactPoint, DepthHitResult.ImpactPoint) : 0.f;
	const float VaultHeight = DepthHitResult.bBlockingHit && VaultHitResult.bBlockingHit ? DepthHitResult.ImpactPoint.Z - VaultHitResult.ImpactPoint.Z : 0.f;
	
	FVector Velocity = GetUmbraCharacter()->GetVelocity();
	Velocity.Z = 0;
	const float Speed = Velocity.Length();
	
	UE_LOG(UmbraAbilitiesLog, Log, TEXT("Vault-able obstacle height = [%f]"), ObstacleHeight);
	UE_LOG(UmbraAbilitiesLog, Log, TEXT("Vault-able obstacle depth = [%f]"), ObstacleDepth);
	UE_LOG(UmbraAbilitiesLog, Log, TEXT("Vault height = [%f]"), VaultHeight);
	UE_LOG(UmbraAbilitiesLog, Log, TEXT("Speed = [%f]"), Speed);

	TMap<FGameplayTag, FTraversalAction> SuitableActionsMap;
	if (!GetTraversalActions()->FindActionsByTag(SuitableActionsMap, FUmbraGameplayTags::Get().Traversal_Vault))
	{ 
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("There are not valid vault actions in traversal actions data"));
		return false;
	}

	for (const auto& Pair : SuitableActionsMap)
	{
		const float MinObstacleHeight = Pair.Value.ObstacleHeightRange.X;
		const float MaxObstacleHeight = Pair.Value.ObstacleHeightRange.Y;

		const float MinObstacleDepth = Pair.Value.ObstacleDepthRange.X;
		const float MaxObstacleDepth = Pair.Value.ObstacleDepthRange.Y;

		const float MinVaultHeight = Pair.Value.VaultHeighRange.X;
		const float MaxVaultHeight = Pair.Value.VaultHeighRange.Y;

		const float MinSpeed = Pair.Value.SpeedRange.X;
		const float MaxSpeed = Pair.Value.SpeedRange.Y;
		
		if (!FMath::IsWithinInclusive(ObstacleHeight, MinObstacleHeight, MaxObstacleHeight)	||
			!FMath::IsWithinInclusive(ObstacleDepth, MinObstacleDepth, MaxObstacleDepth)		||
			!FMath::IsWithinInclusive(VaultHeight, MinVaultHeight, MaxVaultHeight)				||
			!FMath::IsWithinInclusive(Speed, MinSpeed, MaxSpeed))
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
