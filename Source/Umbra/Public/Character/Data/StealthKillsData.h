// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "StealthKillsData.generated.h"

USTRUCT(BlueprintType)
struct FStealthKillMontages
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
struct FStealthKillMontagesContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FStealthKillMontages> Container;
};

/**
 * 
 */
UCLASS()
class UMBRA_API UStealthKillsData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FStealthKillMontagesContainer> StealthKillsData;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FStealthKillMontages GetRandomStealthKillMontagesForPosition(const FGameplayTag& Position);

private:

	FStealthKillMontages GetRandomStealthKillFromContainer(const FStealthKillMontagesContainer& StealthKillMontagesContainer);
};