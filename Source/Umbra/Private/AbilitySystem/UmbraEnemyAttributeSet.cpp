// Copyrighted by Vorona Games


#include "AbilitySystem/UmbraEnemyAttributeSet.h"

#include <string>

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UUmbraEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UUmbraEnemyAttributeSet, Fearfulness, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUmbraEnemyAttributeSet, Aggressiveness, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUmbraEnemyAttributeSet, VisualAnalysis, COND_None, REPNOTIFY_Always);
}

void UUmbraEnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if(Data.EvaluatedData.Attribute == GetFearfulnessAttribute())
	{
		//FString Message = "Changed Fearfulness on " + Data.Target.GetName() + ", Fearfulness = " + FString::SanitizeFloat(GetFearfulness()); 
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
	}
}
