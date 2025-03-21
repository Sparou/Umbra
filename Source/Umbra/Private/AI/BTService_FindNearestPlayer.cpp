// Copyrighted by Vorona Games


#include "AI/BTService_FindNearestPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);
	
	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	for(AActor* Actor : ActorsWithTag)
	{
		if(IsValid(Actor) && IsValid(OwningPawn))
		{
			const float Distance = OwningPawn->GetDistanceTo(Actor);
			if(Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = Actor;
			}
		}
	}
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if(!BlackboardComponent) return;

	BlackboardComponent->SetValueAsObject(TargetToFollow.SelectedKeyName, ClosestActor);
	BlackboardComponent->SetValueAsFloat(DistanceToTarget.SelectedKeyName, ClosestDistance);
}
