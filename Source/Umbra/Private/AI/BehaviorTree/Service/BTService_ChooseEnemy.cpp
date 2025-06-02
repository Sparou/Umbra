// Copyrighted by Vorona Games


#include "AI/BehaviorTree/Service/BTService_ChooseEnemy.h"

#include "AbilitySystemComponent.h"
#include "AI/UmbraAIController.h"

void UBTService_ChooseEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AUmbraAIController* AIController = Cast<AUmbraAIController>(OwnerComp.GetAIOwner());
	if(!AIController) return;

	APawn* AIPawn = AIController->GetPawn();
	if(!AIPawn) return;

	UAbilitySystemComponent* ASC = AIPawn->FindComponentByClass<UAbilitySystemComponent>();
	if (ASC)
	{
		const FGameplayTagContainer& Tags = ASC->GetOwnedGameplayTags();

		for (const FGameplayTag& Tag : Tags)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameplayTag: %s"), *Tag.ToString());
		}
	}

	AIController->ChooseEnemy();
}
