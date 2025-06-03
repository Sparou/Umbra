// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DistantAttack.generated.h"

struct FAbilityEndedData;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class UMBRA_API UBTTask_DistantAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_DistantAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = Node)
	FGameplayTag DistantAttackTag;
	
	UPROPERTY()
	UAbilitySystemComponent* CachedASC;

	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;

	UFUNCTION()
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
};
