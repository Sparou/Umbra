// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Task/BTTask_MeleeAttack.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = "Melee Attack";
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(!AIController) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if(!AIPawn) return EBTNodeResult::Failed;

	UAbilitySystemComponent* AbilitySystemComponent = AIPawn->FindComponentByClass<UAbilitySystemComponent>();
	if(!AbilitySystemComponent) return EBTNodeResult::Failed;

	CachedOwnerComp = &OwnerComp;
	CachedASC = AbilitySystemComponent;

	if(AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(MeleeAttackTag)))
	{
		AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &UBTTask_MeleeAttack::OnAbilityEnded);

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_MeleeAttack::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if(!AbilityEndedData.AbilityThatEnded || !CachedOwnerComp || !CachedASC)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
		return;
	}

	if(AbilityEndedData.AbilityThatEnded->GetAssetTags().HasTag(MeleeAttackTag))
	{
		CachedASC->OnAbilityEnded.RemoveAll(this);

		const EBTNodeResult::Type Result = AbilityEndedData.bWasCancelled ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;
		FinishLatentTask(*CachedOwnerComp, Result);
	}
}
