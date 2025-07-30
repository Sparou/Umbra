// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Traversal/MantleAbility.h"

#include "MotionWarpingComponent.h"
#include "UmbraGameplayTags.h"
#include "Character/UmbraBaseCharacter.h"
#include "Character/Data/TraversalActionsData.h"

bool UMantleAbility::FindTraversalActionMontage(const FHitResult& TopHitResult)
{
	const float TopZ = TopHitResult.ImpactPoint.Z;
	const float RootZ = GetUmbraCharacter()->GetMesh()->GetSocketLocation("root").Z;
	const float ObstacleHeight = TopZ - RootZ;

	UE_LOG(UmbraAbilitiesLog, Log, TEXT("Mantle-able obstacle height = [%f]"), ObstacleHeight);

	TMap<FGameplayTag, FTraversalAction> SuitableActionsMap;
	if (!GetTraversalActions()->FindActionsByTag(SuitableActionsMap, FUmbraGameplayTags::Get().Traversal_Mantle))
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("There are not valid mantle actions in traversal actions data"));
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
