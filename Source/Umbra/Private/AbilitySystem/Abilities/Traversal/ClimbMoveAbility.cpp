// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Traversal/ClimbMoveAbility.h"

#include "AbilitySystemComponent.h"
#include "UmbraGameplayTags.h"
#include "AbilitySystem/GameplayEffect/UmbraGameplayEffectContext.h"
#include "Components/CapsuleComponent.h"

struct FUmbraGameplayEffectContext;

bool UClimbMoveAbility::ValidateClimbMove(float Direction)
{
	bool bSurface = ValidateClimbMoveSurface(Direction);
	bool bLedge = ValidateClimbMoveLedge(Direction);

	if (bSurface && bLedge)
	{
		TopHitResult = NextTopHitResult;
		return true;
	}

	return false;
}

bool UClimbMoveAbility::ValidateClimbMoveSurface(float Direction)
{
	float CapsuleHalfHeight = 0;
	float CapsuleHalfRadius = 0;

	if (UCapsuleComponent* CapsuleComponent = GetCapsuleComponent())
	{
		CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
		CapsuleHalfRadius = CapsuleComponent->GetScaledCapsuleRadius() / 2.f;
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());

	const FVector CurrentTopLocation = TopHitResult.ImpactPoint;
	const FVector CurrentSurfaceLocation = CurrentTopLocation + GetAvatarActorFromActorInfo()->GetActorForwardVector() * MovementSurfaceValidationXOffset + FVector::UpVector * MovementSurfaceValidationZOffset;
	
	for (float Offset = MovementSurfaceValidationStep; Offset <= MovementSurfaceValidationOffset; Offset += MovementSurfaceValidationStep)
	{
		const FVector OverlapLocation = CurrentSurfaceLocation + GetAvatarActorFromActorInfo()->GetActorRightVector() * Direction * Offset;

		if (!HasEnoughSpace(CapsuleHalfHeight, CapsuleHalfRadius, OverlapLocation, ActorsToIgnore, bMovementDebug))
		{
			return false;
		}
	}
	
	return true;
}

bool UClimbMoveAbility::ValidateClimbMoveLedge(float Direction)
{
	const FVector TraceStart = TopHitResult.TraceStart + GetAvatarActorFromActorInfo()->GetActorRightVector() * Direction * MovementLedgeValidationDistance;
	const FVector TraceEnd = TraceStart + FVector::DownVector * MovementLedgeValidationTraceHeight;

	UWorld* World = GetWorld();

	if (!World)
	{
		return false;
	}

	bool bHit = World->LineTraceSingleByChannel(NextTopHitResult, TraceStart, TraceEnd, ECC_Visibility, FCollisionQueryParams::DefaultQueryParam);

	if (bMovementDebug)
	{
		DrawDebugLine(World, TraceStart, TraceEnd, FColor::Green, false, 3.f);
	}
	
	if (bHit)
	{
		if (bMovementDebug)
		{
			DrawDebugSphere(World, NextTopHitResult.ImpactPoint, 4, 4, FColor::Red, false, 3.f);
		}
		return true;
	}

	return false;
}

bool UClimbMoveAbility::TryGetCurrentTopHitResult(FHitResult& OutHitResult)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}

	FGameplayTag ClimbStateTag = FUmbraGameplayTags::Get().Traversal_State_Climb;
	FGameplayEffectQuery Query;
	Query.EffectTagQuery = FGameplayTagQuery(FGameplayTagQuery::MakeQuery_MatchAllTags(FGameplayTagContainer(ClimbStateTag)));

	TArray<FActiveGameplayEffectHandle> ActiveClimbEffects = ASC->GetActiveEffects(Query);

	if (ActiveClimbEffects.Num() > 0)
	{
		FActiveGameplayEffectHandle ClimbEffectHandle = ActiveClimbEffects[0];
		
		FGameplayEffectSpec Spec = ASC->GetActiveGameplayEffect(ClimbEffectHandle)->Spec;
		FGameplayEffectContext* BaseContext = Spec.GetContext().Get();
		//check(BaseContext && BaseContext->GetScriptStruct()->IsChildOf(FUmbraGameplayEffectContext::StaticStruct()));
		FUmbraGameplayEffectContext* UmbraContext = static_cast<FUmbraGameplayEffectContext*>(BaseContext);
		
		if (const FHitResult* LedgeHit = UmbraContext->GetTopHitResult())
		{
			OutHitResult = *LedgeHit;
			return true;
		}
	}

	return false;
}


