// Copyrighted by Vorona Games


#include "Character/Data/AssassinationsData.h"

#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"

FAssassinationData UAssassinationsData::GetRandomAssassinationDataForPosition(const FGameplayTag& Position)
{
	if (!Data.Contains(Position))
	{
		UE_LOG(LogTemp, Warning, TEXT("Stealth Kill Data is not contains [%s] position tag!"), *Position.ToString())
		return FAssassinationData();
	}

	return GetRandomAssassinationDataFromContainer(Data[Position]); 
}

FAssassinationData UAssassinationsData::GetRandomAssassinationDataForPositionWithSeed(const FGameplayTag& Position, int32 Seed)
{
	if (!Data.Contains(Position))
	{
		UE_LOG(LogTemp, Warning, TEXT("Stealth Kill Data is not contains [%s] position tag!"), *Position.ToString())
		return FAssassinationData();
	}

	return GetRandomAssassinationDataFromContainerWithSeed(Data[Position], Seed);
}

FAssassinationData UAssassinationsData::GetRandomAssassinationDataFromAbility(
	FGameplayAbilityActivationInfo ActivationInfo, const FGameplayTag& Position, float SeedMultiplier)
{
	if (Data[Position].Container.Num() <= 0)
	{
		return FAssassinationData();
	}
	
	FRandomStream RandomStream(ActivationInfo.GetActivationPredictionKey().Current * SeedMultiplier);
	return Data[Position].Container[RandomStream.RandRange(0, Data[Position].Container.Num() - 1)];
}


FAssassinationData UAssassinationsData::GetRandomAssassinationDataFromContainer(const FAssassinationDataContainer& AssassinationDataContainer)
{
	if (AssassinationDataContainer.Container.Num() <= 0)
	{
		return FAssassinationData();
	}
  
	return AssassinationDataContainer.Container[FMath::RandRange(0, AssassinationDataContainer.Container.Num() - 1)];
}

FAssassinationData UAssassinationsData::GetRandomAssassinationDataFromContainerWithSeed(const FAssassinationDataContainer& AssassinationDataContainer, int32 Seed)
{
	if (AssassinationDataContainer.Container.Num() <= 0)
	{
		return FAssassinationData();
	}

	FRandomStream RandomStream(Seed);
	return AssassinationDataContainer.Container[RandomStream.RandRange(0, AssassinationDataContainer.Container.Num() - 1)];
}
