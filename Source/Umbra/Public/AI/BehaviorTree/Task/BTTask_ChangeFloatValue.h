// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChangeFloatValue.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UBTTask_ChangeFloatValue : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ChangeFloatValue();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector BlackboardKey;

	UPROPERTY(EditAnywhere, Category = Node)
	float AddNumber;
};
