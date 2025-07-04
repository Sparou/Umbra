// Copyrighted by Vorona Games


#include "AI/UmbraAIController.h"

#include "UmbraGameplayTags.h"
#include "AbilitySystem/UmbraEnemyAttributeSet.h"
#include "AI/UmbraAIPerceptionComponent.h"
#include "AI/Data/DA_EnemyChoicePriority.h"
#include "AI/Data/FEmotionReactionRow.h"
#include "AI/InteractingObject/UmbraAlarmBell.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/UmbraEnemyCharacter.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Character/Component/TagManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Stealth/LightingDetection.h"

AUmbraAIController::AUmbraAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(Blackboard);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BehaviorTreeComponent);

	UmbraAIPerceptionComponent = CreateDefaultSubobject<UUmbraAIPerceptionComponent>("UmbraAIPerceptionComponent");
	check(UmbraAIPerceptionComponent);
	SetPerceptionComponent(*UmbraAIPerceptionComponent);
	
	UmbraAIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AUmbraAIController::OnPercepted);
	
	//NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>("NoiseEmitter");
	//NoiseEmitter->bAutoActivate = true;
}

bool AUmbraAIController::InitializeBlackboardDefaultValues(const UUmbraEnemyAttributeSet* AttributeSet) const
{
	if(!AttributeSet) return false;
	//TODO: check set variables on validity
	
	Blackboard->SetValueAsFloat(Belligerence, AttributeSet->GetAggressiveness());
	Blackboard->SetValueAsFloat(Fear, AttributeSet->GetFearfulness());
	Blackboard->SetValueAsFloat(EstimatedWinChance, AttributeSet->GetVisualAnalysis());
	Blackboard->SetValueAsBool(IsActiveName, Cast<AUmbraEnemyCharacter>(GetCharacter())->bIsActive);

	return true;
}

bool AUmbraAIController::ReactToEvent(const FName EventName)
{
	if(!EmotionReactionTable) return false;
	if(!EmotionModifierTable) return false;

	const FEmotionReactionRow* ReactionRow = EmotionReactionTable->FindRow<FEmotionReactionRow>(EventName, TEXT("EmotionReaction"));
	if(!ReactionRow) return false;
	
	AUmbraEnemyCharacter* EnemyCharacter = Cast<AUmbraEnemyCharacter>(GetPawn());
	if(!EnemyCharacter) return false;

	const UAbilitySystemComponent* AbilitySystemComponent = EnemyCharacter->GetAbilitySystemComponent();
	if(!AbilitySystemComponent) return false;

	const UUmbraEnemyAttributeSet* AttributeSet = AbilitySystemComponent->GetSet<UUmbraEnemyAttributeSet>();
	if(!AttributeSet) return false;

	const FRealCurve* AggressivenessModifierCurve = EmotionModifierTable->FindCurve(FName("Aggressiveness"), TEXT("EmotionModifier"));
	if(!AggressivenessModifierCurve) return false;

	float AggressivenessModifier = AggressivenessModifierCurve->Eval(AttributeSet->GetAggressiveness());

	FRealCurve* FearfulnessModifierCurve = EmotionModifierTable->FindCurve(FName("Fearfulness"), TEXT("EmotionModifier"));
	if(!FearfulnessModifierCurve) return false;

	float FearfulnessModifier = FearfulnessModifierCurve->Eval(AttributeSet->GetFearfulness());

	const float NewBelligerenceValue = Blackboard->GetValueAsFloat(Belligerence) + ReactionRow->BelligerenceDelta * AggressivenessModifier;
	const float NewFearValue = Blackboard->GetValueAsFloat(Fear) + ReactionRow->FearDelta * FearfulnessModifier;
	Blackboard->SetValueAsFloat(Belligerence, NewBelligerenceValue);
	Blackboard->SetValueAsFloat(Fear, NewFearValue);	
	
	return true;
}

bool AUmbraAIController::ChooseEnemy()
{
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if(!BlackboardComponent) return false;

	float BestPriority = 0;
	const TTuple<AActor*, FEnemyData>* BestEnemy = nullptr;
	const AActor* CurEnemy = Cast<AActor>(BlackboardComponent->GetValueAsObject(CurrentEnemy));

	const auto& [ThreatWeight,
				DistanceWeight,
				TimeSinceSeenWeight,
				IsCurrentTargetBonus] = PriorityWeightsData->PriorityWeights;
	const auto& [MaxThreatLevel,
				MaxDistance,
				MaxTimeNotSeen] = PriorityWeightsData->PriorityMaxValues;
	
	//Calculating Priority function
	for(const TTuple<AActor*, FEnemyData>& EnemyData : KnownEnemies)
	{
		const float Distance = GetPawn()->GetDistanceTo(EnemyData.Key);
		const float LastSeenTimeInterval = GetWorld()->GetTimeSeconds() - EnemyData.Value.LastSeenTime;
		const bool IsCurrentEnemy = CurEnemy == EnemyData.Key;

		const float NormalizedThreat = FMath::Clamp(EnemyData.Value.ThreatLevel / MaxThreatLevel, 0.f, 1.f);
		const float NormalizedDistance = 1.f - FMath::Clamp(Distance / MaxDistance, 0.f, 1.f);
		const float NormalizedLastSeenTimeInterval = 1.f - FMath::Clamp(LastSeenTimeInterval / MaxTimeNotSeen, 0.f, 1.f);
		const float CurrentEnemyBonus = IsCurrentEnemy ? 1.f : 0.f;

		const float Priority = ThreatWeight * NormalizedThreat +
								DistanceWeight * NormalizedDistance +
								TimeSinceSeenWeight * NormalizedLastSeenTimeInterval +
								IsCurrentTargetBonus * CurrentEnemyBonus;

		//UE_LOG(LogTemp, Display, TEXT("Player %s have priority %f: threat = %f, distance = %f, time = %f, curTargetBon = %f"),
		//	*EnemyData.Key->GetName(),
		//	Priority,
		//	ThreatWeight * NormalizedThreat,
		//	DistanceWeight * NormalizedDistance,
		//	TimeSinceSeenWeight * NormalizedLastSeenTimeInterval,
		//	IsCurrentTargetBonus * CurrentEnemyBonus);
		FString Message = "Player" + EnemyData.Key->GetName() + "have priority" + FString::SanitizeFloat(Priority) +
			": threat = " + FString::SanitizeFloat(ThreatWeight * NormalizedThreat) + ", distance = " +
				FString::SanitizeFloat(DistanceWeight * NormalizedDistance) + ", time = " +
					FString::SanitizeFloat(TimeSinceSeenWeight * NormalizedLastSeenTimeInterval) + ", curTargetBon = " +
						FString::SanitizeFloat(IsCurrentTargetBonus * CurrentEnemyBonus);
		

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
		if(Priority > BestPriority)
		{
			BestPriority = Priority;
			BestEnemy = &EnemyData;
		}
	}

	if(BestEnemy == nullptr)
	{
		return false;
	}
	
	//if see enemy
	if(BestEnemy->Value.IsVisible || BestEnemy->Value.LastSeenTime < ForgettingTime)
	{
		if(Cast<AActor>(BlackboardComponent->GetValueAsObject(CurrentEnemy)) != BestEnemy->Key)
		{
			BlackboardComponent->SetValueAsObject(CurrentEnemy, BestEnemy->Key);
			this->SetFocus(Cast<AActor>(BestEnemy->Key));
		}
	}
	else
	{
		BlackboardComponent->ClearValue(CurrentEnemy);
		this->ClearFocus(EAIFocusPriority::Gameplay);
		BlackboardComponent->SetValueAsObject(LostEnemy, BestEnemy->Key);
		BlackboardComponent->SetValueAsVector(EnemyLocation, BestEnemy->Value.LastKnownLocation);
	}

	return true;
}

void AUmbraAIController::BeginPlay()
{
	Super::BeginPlay();

	AUmbraBaseCharacter* UmbraCharacter = Cast<AUmbraBaseCharacter>(GetCharacter());
	UmbraCharacter->GetTagManager()->OnGameplayTagChanged.AddDynamic(this, &AUmbraAIController::OnGameplayTagChanged);
	//UmbraCharacter->GetAbilitySystemComponent()->RegisterGameplayTagEvent()
}

void AUmbraAIController::OnPercepted(AActor* SourceActor, const FAIStimulus Stimulus)
{
	//FString Message = "Stimuled by" + Stimulus.Type.Name.ToString() + "strength = " + FString::SanitizeFloat(Stimulus.Strength);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
	if(!SourceActor) return;
	
	if(Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
	{
		if(Stimulus.WasSuccessfullySensed())
		{
			if(AUmbraPlayerCharacter* PlayerActor = Cast<AUmbraPlayerCharacter>(SourceActor))
			{
				const float light = PlayerActor->GetLightingDetector()->LightPercentage;
				//UE_LOG(LogTemp, Warning, TEXT("Light = %s"), *FString::SanitizeFloat(light))
				//TODO: check if bot sees player after ending invisibility when stimulus was already received 
				if(true/*!PlayerActor->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(InvisibleTag))*/)
				{
					if(!Blackboard->GetValueAsBool(EverSeenEnemy))
					{
						Blackboard->SetValueAsBool(EverSeenEnemy, true);
					}

					const bool IsNewEnemy = !KnownEnemies.Contains(SourceActor);
					const FEnemyData EnemyData(Stimulus.StimulusLocation,
						GetWorld()->GetTimeSeconds(),
						true,
						IsNewEnemy ? 0 : KnownEnemies[SourceActor].ThreatLevel);
					if(IsNewEnemy)
					{
						KnownEnemies.Add(SourceActor, EnemyData);
						ReactToEvent(SeeEnemy);
					}
					else
					{
						KnownEnemies[SourceActor] = EnemyData;
					}
					
					/* assignment in case ChooseEnemy service haven't yet worked out */
					if(!Blackboard->GetValueAsObject(CurrentEnemy))
					{
						//ChooseEnemy();
					}
				}
			}
			if(AUmbraEnemyCharacter* EnemyActor = Cast<AUmbraEnemyCharacter>(SourceActor))
			{
				if(ICombatInterface::Execute_IsDead(EnemyActor))
				{
					//TODO:
					//ReactToEvent(SeeCorpse);
					//Blackboard->SetValueAsBool(bSeeCorpse)
					//Blackboard->SetValueAsVector(CorpseLocation)
				}
				else
				{
					ReactToEvent(SeeAlly);
				}
			}
		}
		else
		{
			if(AUmbraPlayerCharacter* PlayerActor = Cast<AUmbraPlayerCharacter>(SourceActor))
			{
				if(KnownEnemies.Contains(SourceActor))
				{
					KnownEnemies[SourceActor].IsVisible = false;
					KnownEnemies[SourceActor].LastKnownLocation = Stimulus.StimulusLocation;
					KnownEnemies[SourceActor].LastSeenTime = GetWorld()->GetTimeSeconds();
				}
			}
		}
	}

	if(Stimulus.Type == UAISense::GetSenseID(UAISense_Hearing::StaticClass()))
	{
		if(Stimulus.WasSuccessfullySensed())
		{
			if(AUmbraPlayerCharacter* PlayerActor = Cast<AUmbraPlayerCharacter>(SourceActor))
			{
				if(true/*!PlayerActor->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(SilenceTag))*/)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Sound emitted by %s"), *SourceActor->GetName());
					Blackboard->SetValueAsVector(SoundLocation, Stimulus.StimulusLocation);
					//DrawDebugSphere(GetWorld(), Stimulus.StimulusLocation, 5.f, 6, FColor::Red, false, 1.f, 0, 1.f);

					if(!KnownEnemies.Contains(SourceActor))
					{
						ReactToEvent(HearNoise);
					}
				}
			}
		}
		else
		{
		}
	}
	
}

void AUmbraAIController::OnGameplayTagChanged(const FGameplayTag& Tag, bool bAdded)
{
	//TODO Ability.TrapBlock
	
	if(!bAdded) return;
	AUmbraBaseCharacter* UmbraCharacter = Cast<AUmbraBaseCharacter>(GetCharacter());
	UmbraCharacter->GetCharacterMovement()->MaxWalkSpeed =
		UmbraCharacter->GetMoveSpeed(FUmbraGameplayTags::Get().State_Stance_Standing, Tag);
}