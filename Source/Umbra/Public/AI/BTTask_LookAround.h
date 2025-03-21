// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LookAround.generated.h"

class UUmbraBlackboardComponent;
/**
 * 
 */
UCLASS()
class UMBRA_API UBTTask_LookAround : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_LookAround();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	

protected:
	UPROPERTY(EditAnywhere, Category = "Time")
	float OneSideAttentionTime = 2.f;

	UPROPERTY(EditAnywhere, Category = "Time")
	float LookAroundTime = 6.f;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationAngle = 45.f;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed = 5.f;

private:
	float ElapsedTime = 0.f;
	float SideTime = 0.f;
	bool bLookingRight = true;
	FRotator OriginalRotation;
	FRotator TargetRotation;
};
