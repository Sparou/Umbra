// Copyrighted by Vorona Games


#include "Character/Data/TraversalActionsData.h"

#include "GameplayAbilitySpec.h"

bool UTraversalActionsData::FindActionByTag(FTraversalAction& OutTraversalAction, const FGameplayTag& ActionTypeTag) const
{
	OutTraversalAction = ActionsMap.FindRef(ActionTypeTag);

	if (OutTraversalAction.Montages.Num() <= 0)
	{
		return false;
	}

	return true;
}

bool UTraversalActionsData::FindActionsByTag(TMap<FGameplayTag, FTraversalAction>& OutSuitableActionsMap, const FGameplayTag& ActionTypeTag) const
{
	for (const auto& Pair : ActionsMap)
	{
		if (Pair.Key.MatchesTag(ActionTypeTag))
		{
			OutSuitableActionsMap.Add(Pair.Key, Pair.Value);
		}
	}

	if (OutSuitableActionsMap.Num() <= 0)
	{
		return false;
	}

	return true;
}

bool UTraversalActionsData::GetRandomTraversalActionFromAbility(FTraversalActionMontage& OutTraversalAction, const FGameplayTag& ActionTypeTag, const FGameplayAbilityActivationInfo& AbilityActivationInfo, float SeedMultiplier)
{
	const FTraversalAction* TraversalAction = ActionsMap.Find(ActionTypeTag);

	if (!TraversalAction || TraversalAction->Montages.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("UTraversalActionsData::GetRandomTraversalActionFromAbility: No actions found for tag [%s] or the array is empty."), *ActionTypeTag.ToString());
		return false;
	}
	
	FRandomStream RandomStream(AbilityActivationInfo.GetActivationPredictionKey().Current * SeedMultiplier);
	const int32 RandomIndex = RandomStream.RandRange(0, TraversalAction->Montages.Num() - 1);
	OutTraversalAction = TraversalAction->Montages[RandomIndex];

	if (!OutTraversalAction.Montage)
	{
		return false;
	}

	return true;
}

