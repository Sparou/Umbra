// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToDynamicLocation.generated.h"

UCLASS()
class UMBRA_API UBTTask_MoveToDynamicLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToDynamicLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius = 5.f;
	
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ToolTip = "the distance from the old location value to the new one at which the path will be updated"))
	float PathUpdateThreshold = 100.f;

private:
	FVector PreviousTargetLocation = FVector::ZeroVector;
	bool bIsMoving = false;

	FORCEINLINE bool ShouldUpdatePath(const FVector& NewLocation) const
	{
		return FVector::DistSquared(NewLocation, PreviousTargetLocation) > FMath::Square(PathUpdateThreshold);
	}
};
