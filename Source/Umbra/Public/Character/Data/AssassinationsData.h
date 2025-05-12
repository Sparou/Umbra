// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AssassinationsData.generated.h"

USTRUCT(BlueprintType)
struct FAssassinationData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* KillerMontage = nullptr;
  
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* VictimMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector KillerStarterLocation;
};


USTRUCT()
struct FAssassinationDataContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FAssassinationData> Container;
};

/**
 * 
 */
UCLASS()
class UMBRA_API UAssassinationsData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FAssassinationDataContainer> Data;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FAssassinationData GetRandomAssassinationDataForPosition(const FGameplayTag& Position);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FAssassinationData GetRandomAssassinationDataForPositionWithSeed(const FGameplayTag& Position, int32 Seed);

private:

	FAssassinationData GetRandomAssassinationDataFromContainer(const FAssassinationDataContainer& AssassinationDataContainer);

	FAssassinationData GetRandomAssassinationDataFromContainerWithSeed(const FAssassinationDataContainer& AssassinationDataContainer, int32 Seed);
};
