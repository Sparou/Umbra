// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindNextPatrollingPoint.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UBTTask_FindNextPatrollingPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindNextPatrollingPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Patrolling")
	FBlackboardKeySelector CurrentDestinationPoint;
};
