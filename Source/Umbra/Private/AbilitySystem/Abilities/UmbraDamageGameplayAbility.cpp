// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/UmbraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UmbraGameplayTags.h"
#include "AI/UmbraAIController.h"

void UUmbraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, FUmbraGameplayTags::Get().Combat_Damage, ScaledDamage);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));

	//AI react to damage logic
	AController* Controller = TargetActor->GetInstigatorController();
	if(Controller && Controller->IsA<AUmbraAIController>())
	{
		Cast<AUmbraAIController>(Controller)->ReactToEvent("TakeDamage");
	}
}

void UUmbraDamageGameplayAbility::WeaponTrace(TArray<AActor*>& OutHitActors,
	const TArray<AActor*>& ActorsToIgnore, const FVector& TraceStart, const FVector& TraceEnd, float Radius, bool bDrawDebug)
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

	if (!bHit && bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), TraceStart, 5.f, 10, FColor::Red, false, 3.f);
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 3.f, 0, 2);
		DrawDebugSphere(GetWorld(), TraceEnd, 5.f, 10, FColor::Red, false, 3.f);
	}
	if (bHit && bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), TraceStart, 5.f, 10, FColor::Green, false, 3.f);
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 3.f, 0, 2);
		DrawDebugSphere(GetWorld(), TraceEnd, 5.f, 10, FColor::Green, false, 3.f);
	}
	
	for (const auto& Hit : Hits)
	{
		OutHitActors.AddUnique(Hit.GetActor());
	}
}
