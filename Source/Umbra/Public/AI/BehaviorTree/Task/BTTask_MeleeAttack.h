// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MeleeAttack.generated.h"

struct FGameplayAbilitySpecHandle;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class UMBRA_API UBTTask_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MeleeAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = Node)
	FGameplayTag MeleeAttackTag;
	
	UPROPERTY()
	UAbilitySystemComponent* CachedASC;

	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;

	UFUNCTION()
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
};
