// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SyncLocomotionTag.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UBTService_SyncLocomotionTag : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SyncLocomotionTag();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;

	UPROPERTY(EditAnywhere, Category = Node)
	FBlackboardKeySelector IsRunningKey;

	UPROPERTY(EditAnywhere, Category = "Tag")
	FGameplayTag WalkingTag;

	UPROPERTY(EditAnywhere, Category = "Tag")
	FGameplayTag RunningTag;

private:
	struct FBTService_SyncMemory
	{
		bool bIsRunning;
	};
};
