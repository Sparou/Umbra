// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTService_UpdateCurrentEnemyData.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UBTService_UpdateCurrentEnemyData : public UBTService_BlueprintBase
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBlackboardKeySelector CurrentEnemy;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBlackboardKeySelector DistanceToTarget;
};
