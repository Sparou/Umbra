// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Ring.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UBTTask_Ring : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Node", meta = (ClampMin = "0.0"))
	float AcceptanceRadius = 100.f;
};
