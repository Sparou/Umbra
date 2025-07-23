// Copyrighted by Vorona Games


#include "Actor/UmbraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UmbraGameplayTags.h"

DEFINE_LOG_CATEGORY(UmbraEffectActorLog);

AUmbraEffectActor::AUmbraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUmbraEffectActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	TArray<TWeakObjectPtr<AActor>> TargetActorsToClean;
	ActiveEffects.GetKeys(TargetActorsToClean);

	for (TWeakObjectPtr<AActor> TargetActor : TargetActorsToClean)
	{
		if (!TargetActor.IsValid())
		{
			continue;
		}
		RemoveActiveEffectFromTarget(TargetActor.Get());
	}

	ActiveEffects.Empty();
	Super::EndPlay(EndPlayReason);
}

void AUmbraEffectActor::ApplyEffectToTarget(AActor* TargetActor, FOverlapEffectData& OverlapEffect)
{
	UAbilitySystemComponent* const ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (ASC == nullptr)
	{
		UE_LOG(UmbraEffectActorLog, Warning, TEXT("[%s]: ASC is null for TargetActor [%s]. Cannot apply effect."), *GetNameSafe(this), *GetNameSafe(TargetActor));
		return;
	}
	
	if ((ASC->HasMatchingGameplayTag(FUmbraGameplayTags::Get().Character_Player) && OverlapEffect.TargetTypePolicy == EEffectTargetTypePolicy::ApplyOnEnemies) ||
		(ASC->HasMatchingGameplayTag(FUmbraGameplayTags::Get().Character_Enemy) && OverlapEffect.TargetTypePolicy == EEffectTargetTypePolicy::ApplyOnPlayers))
	{
		UE_LOG(UmbraEffectActorLog, Log, TEXT("[%s]: Skipping effect application to [%s] due to target type policy."), *GetNameSafe(this), *GetNameSafe(TargetActor));
		return;
	}

	if (OverlapEffect.GameplayEffectClass == nullptr)
	{
		UE_LOG(UmbraEffectActorLog, Warning, TEXT("GameplayEffect is not valid in [%s]"), *GetNameSafe(this));
		return;
	}

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(OverlapEffect.GameplayEffectClass, OverlapEffect.EffectLevel, EffectContextHandle);
	FActiveGameplayEffectHandle AppliedGameplayEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	OverlapEffect.bIsApplied = true;

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && OverlapEffect.RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		FAppliedOverlapEffectData AppliedOverlapEffectData;
		AppliedOverlapEffectData.GameplayEffectHandle = AppliedGameplayEffectHandle;
		AppliedOverlapEffectData.RemovalPolicy = OverlapEffect.RemovalPolicy;

		TArray<FAppliedOverlapEffectData>& TargetEffectsArray = ActiveEffects.FindOrAdd(TargetActor);
		
		if (TargetEffectsArray.AddUnique(AppliedOverlapEffectData) != INDEX_NONE) 
		{
			UE_LOG(UmbraEffectActorLog, Log, TEXT("[%s]: Applied and tracking Infinite GE [%s] to [%s] with handle [%s]"),
				*GetNameSafe(this), *GetNameSafe(OverlapEffect.GameplayEffectClass), *GetNameSafe(TargetActor), *AppliedGameplayEffectHandle.ToString());
		}
		else
		{
			UE_LOG(UmbraEffectActorLog, Log, TEXT("[%s]: GameplayEffect [%s] already tracked for [%s]. Skipping re-tracking."),
				*GetNameSafe(this), *GetNameSafe(OverlapEffect.GameplayEffectClass), *GetNameSafe(TargetActor));
		}
	}
}

void AUmbraEffectActor::RemoveActiveEffectFromTarget(AActor* TargetActor)
{
	CleanupInvalidTargets();
	
	UAbilitySystemComponent* const ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (ASC == nullptr)
	{
		UE_LOG(UmbraEffectActorLog,
			Warning,
			TEXT("[%s]: ASC is null for TargetActor [%s] during removal. Cannot remove effects."),
			*GetNameSafe(this),
			*GetNameSafe(TargetActor)
		);
		
		ActiveEffects.Remove(TargetActor);
		return;
	}
	
	if (ActiveEffects.Find(TargetActor) == nullptr)
	{
		UE_LOG(UmbraEffectActorLog, Verbose, TEXT("[%s]: No tracked effects found for TargetActor [%s]. Nothing to remove."), *GetNameSafe(this), *GetNameSafe(TargetActor));
		return;
	}

	TArray<FAppliedOverlapEffectData>& TargetEffects = *ActiveEffects.Find(TargetActor);
	
	for (int32 i = TargetEffects.Num() - 1; i >= 0; i--)
	{
		if (TargetEffects[i].RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			ASC->RemoveActiveGameplayEffect(TargetEffects[i].GameplayEffectHandle);
			TargetEffects.RemoveAt(i);
		}
	}

	if (TargetEffects.Num() == 0)
	{
		ActiveEffects.Remove(TargetActor);
		UE_LOG(UmbraEffectActorLog, Log, TEXT("[%s]: All tracked effects for [%s] removed. Clearing map entry."), *GetNameSafe(this), *GetNameSafe(TargetActor));
	}
}

void AUmbraEffectActor::CleanupInvalidTargets()
{
	TArray<TWeakObjectPtr<AActor>> KeysToRemove;
	for (auto& Pair : ActiveEffects)
	{
		if (!Pair.Key.IsValid())
		{
			KeysToRemove.Add(Pair.Key);
			UE_LOG(UmbraEffectActorLog, Log, TEXT("[%s]: Found invalid TargetActor key in map. Preparing to remove."), *GetNameSafe(this));
		}
	}

	for (const TWeakObjectPtr<AActor>& Key : KeysToRemove)
	{
		ActiveEffects.Remove(Key);
	}
}

bool AUmbraEffectActor::CheckAllEffectsAreApplied()
{
	for (const auto& OverlapEffect : OverlapEffects)
	{
		if (OverlapEffect.bIsApplied == false)
		{
			return false;
		}
	}

	return true;
}

void AUmbraEffectActor::OnBeginOverlap(AActor* TargetActor)
{
	for (FOverlapEffectData& OverlapEffect : OverlapEffects)
	{
		if (OverlapEffect.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
		{
			ApplyEffectToTarget(TargetActor, OverlapEffect);
		}
	}

	if (DestructionPolicy == EEffectActorDestructionPolicy::WhenAllEffectsApplied && CheckAllEffectsAreApplied())
	{
		Destroy();
	}

	if (DestructionPolicy == EEffectActorDestructionPolicy::OnActorBeginOverlap)
	{
		Destroy();
	}
}

void AUmbraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	
	for (FOverlapEffectData& OverlapEffect : OverlapEffects)
	{
		if (OverlapEffect.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, OverlapEffect);
		}
	}

	if (DestructionPolicy == EEffectActorDestructionPolicy::WhenAllEffectsApplied && CheckAllEffectsAreApplied())
	{
		Destroy();
	}
	
	if (DestructionPolicy == EEffectActorDestructionPolicy::OnActorEndOverlap)
	{
		Destroy();
	}

	RemoveActiveEffectFromTarget(TargetActor);
}



