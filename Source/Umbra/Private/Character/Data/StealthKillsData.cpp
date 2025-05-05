// Copyrighted by Vorona Games


#include "Character/Data/StealthKillsData.h"

FStealthKillMontages UStealthKillsData::GetRandomStealthKillMontagesForPosition(const FGameplayTag& Position)
{
	if (!StealthKillsData.Contains(Position))
	{
		UE_LOG(LogTemp, Warning, TEXT("Stealth Kill Data is not contains [%s] position tag!"), *Position.ToString())
		return FStealthKillMontages();
	}

	return GetRandomStealthKillFromContainer(StealthKillsData[Position]); 
}

FStealthKillMontages UStealthKillsData::GetRandomStealthKillFromContainer(
  const FStealthKillMontagesContainer& StealthKillMontagesContainer)
{

	if (StealthKillMontagesContainer.Container.Num() <= 0)
	{
		return FStealthKillMontages();
	}
  
	return StealthKillMontagesContainer.Container[FMath::RandRange(0, StealthKillMontagesContainer.Container.Num() - 1)];
}