// Copyrighted by Vorona Games


#include "Character/UmbraEnemyCharacter.h"
#include "AbilitySystem/UmbraEnemyAttributeSet.h"
#include "AI/UmbraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AUmbraEnemyCharacter::AUmbraEnemyCharacter()
{
	AttributeSet = CreateDefaultSubobject<UUmbraEnemyAttributeSet>("Attribute Set");
}

void AUmbraEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority()) return;

	UmbraAIController = Cast<AUmbraAIController>(NewController);
	UmbraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	UmbraAIController->RunBehaviorTree(BehaviorTree);
}

void AUmbraEnemyCharacter::Interact(AActor* OtherActor)
{
	bInInteraction = true;
	GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Blue, FString("Interacted with object"));
	bInInteraction = false;
}

AActor* AUmbraEnemyCharacter::GetCurrentDestinationPoint() const
{
	//Check index's variables validity
	if(PatrollingPaths.IsValidIndex(CurrentPathIndex) &&
		PatrollingPaths[CurrentPathIndex].PathPoints.IsValidIndex(CurrentDestinationPointIndex))
	{
		return PatrollingPaths[CurrentPathIndex].PathPoints[CurrentDestinationPointIndex];
	}
	return nullptr;
}

void AUmbraEnemyCharacter::IncrementCurrentDestinationPoint()
{
	//Check index's variables validity
	if(PatrollingPaths.IsValidIndex(CurrentPathIndex) &&
		PatrollingPaths[CurrentPathIndex].PathPoints.IsValidIndex(CurrentDestinationPointIndex + 1))
	{
		CurrentDestinationPointIndex++;
	}
	else
	{
		ChoosePath();
	}
}

void AUmbraEnemyCharacter::ChoosePath()
{
	CurrentPathIndex = FMath::RandRange(0, PatrollingPaths.Num() - 1);
	CurrentDestinationPointIndex = 0;
}

void AUmbraEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	ChoosePath();
}

void AUmbraEnemyCharacter::InitializeDefaultAttributes() const
{
	Super::InitializeDefaultAttributes();
	ApplyEffectToSelf(DefaultTemperamentalAttributes, 1.f);
}
