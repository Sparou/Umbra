// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PatrollingInterface.generated.h"

USTRUCT(BlueprintType)
struct FPathData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> PathPoints;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UPatrollingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UMBRA_API IPatrollingInterface
{
	GENERATED_BODY()

public:
	virtual AActor* GetCurrentDestinationPoint() const = 0;
	virtual void IncrementCurrentDestinationPoint() = 0;
	virtual void ChoosePath() = 0;
};
