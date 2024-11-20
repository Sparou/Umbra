// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/UmbraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UUmbraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

void UUmbraDamageGameplayAbility::WeaponTrace(TArray<AActor*>& OutHitActors,
	const TArray<AActor*>& ActorsToIgnore, const FVector& TraceStart, const FVector& TraceEnd, float Radius)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	UWorld* World = GetWorld();
	if (!World) return;

	TArray<FHitResult> Hits;
	bool bHit = World->SweepMultiByObjectType(
		Hits,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
		FCollisionShape::MakeSphere(Radius),
		SphereParams);

	for (const auto& Hit : Hits)
	{
		OutHitActors.AddUnique(Hit.GetActor());
	}
	
#if WITH_EDITOR
	FColor TraceColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugSphere(World, TraceStart, Radius, 12, TraceColor, false, 1.0f);
	DrawDebugSphere(World, TraceEnd, Radius, 12, TraceColor, false, 1.0f);
	DrawDebugLine(World, TraceStart, TraceEnd, TraceColor, false, 1.0f, 0, 1.0f);
#endif
}
