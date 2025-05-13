// Copyrighted by Vorona Games


#include "Character/UmbraEnemyCharacter.h"

#include "UmbraCollisionChannels.h"
#include "AbilitySystem/UmbraEnemyAttributeSet.h"
#include "AI/UmbraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AUmbraEnemyCharacter::AUmbraEnemyCharacter()
{
	AttributeSet = CreateDefaultSubobject<UUmbraEnemyAttributeSet>("Attribute Set");
	GetMesh()->SetCollisionResponseToChannel(ECC_Interaction, ECR_Block);
}

void AUmbraEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority()) return;

	UmbraAIController = Cast<AUmbraAIController>(NewController);
	UmbraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	UmbraAIController->RunBehaviorTree(BehaviorTree);
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

void AUmbraEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	//Cast<UmbraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if(HasAuthority())
	{
		InitializeDefaultAttributes();
		Super::InitAbilityActorInfo();
	}
}

void AUmbraEnemyCharacter::InitializeDefaultAttributes() const
{
	Super::InitializeDefaultAttributes();
	ApplyEffectToSelf(DefaultTemperamentalAttributes, 1.f);
	UmbraAIController->InitializeBlackboardDefaultValues(Cast<UUmbraEnemyAttributeSet>(AttributeSet));
}
