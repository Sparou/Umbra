// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetBoolValue.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UBTTask_SetBoolValue : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SetBoolValue();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BlackboardKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	bool Value;
};
