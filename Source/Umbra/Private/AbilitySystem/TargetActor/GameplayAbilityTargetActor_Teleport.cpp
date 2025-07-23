
#include "AbilitySystem/TargetActor/GameplayAbilityTargetActor_Teleport.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void AGameplayAbilityTargetActor_Teleport::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	if (ReticleActor.IsValid())
	{
		ReticleActor->SetActorHiddenInGame(true);
	}
}

void AGameplayAbilityTargetActor_Teleport::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

FHitResult AGameplayAbilityTargetActor_Teleport::PerformTrace(AActor* InSourceActor)
{
	FHitResult InitialHitResult;
	PerformPrimaryTrace(InitialHitResult, InSourceActor);

	// --- НЕТ ПОПАДАНИЯ ---
	if (!InitialHitResult.bBlockingHit)
	{
		InitialHitResult.Location = HasEnoughSpace(InitialHitResult.TraceEnd, InSourceActor) ? InitialHitResult.TraceEnd : InitialHitResult.TraceStart;
		HideReticle();
		return InitialHitResult;
	}

	// --- ГОРИЗОНТАЛЬНАЯ ПОВЕРХНОСТЬ --- 	
	const ACharacter* Character = Cast<ACharacter>(InSourceActor);
	const float WalkableFloorZ = Character ? Character->GetCharacterMovement()->GetWalkableFloorZ() : 0.7f;
	
	if (InitialHitResult.ImpactNormal.Z >= WalkableFloorZ)
	{

		// Слегка поднимаем капсулу. В случае, если имеем наклонную поверхность это исключает возможность некорректной коллизии.
		if (HasEnoughSpace(InitialHitResult.Location + FVector(0,0,15), InSourceActor))
		{
			ShowReticle(InitialHitResult.Location);
			return InitialHitResult;
		}
		
		HideReticle();	
		InitialHitResult.bBlockingHit = false;
		return InitialHitResult;
	}

	// --- СТЕНА ---
	for (int32 i = WallHorizontalOffsetIterations; i >= 0; i--)
	{
		const FVector HorizontalOffset = InitialHitResult.Location + -InitialHitResult.ImpactNormal * WallHorizontalOffsetStep * i;
		const FVector VerticalTraceStart = HorizontalOffset + FVector::UpVector * MaxWallHeight;
		const FVector VerticalTraceEnd = VerticalTraceStart + FVector::DownVector * MaxWallHeight;

		FHitResult SecondaryHitResult;
		FCollisionQueryParams Params;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(InSourceActor);

		if (ReticleActor.IsValid())
		{
			ActorsToIgnore.Add(ReticleActor.Get());
		}

		Params.AddIgnoredActors(ActorsToIgnore);
		
		LineTraceWithFilter(SecondaryHitResult, GetWorld(), Filter, VerticalTraceStart, VerticalTraceEnd, TraceProfile.Name, Params);

		if (SecondaryHitResult.bBlockingHit && HasEnoughSpace(SecondaryHitResult.Location, InSourceActor))
		{
#if ENABLE_DRAW_DEBUG
			if (bDebug)
			{
				DrawDebugSphere(GetWorld(), VerticalTraceStart, 8.f, 8, FColor::Blue, false, 0);
				DrawDebugSphere(GetWorld(), VerticalTraceEnd, 8.f, 8, FColor::Green, false, 0);
				DrawDebugLine(GetWorld(), VerticalTraceStart, VerticalTraceEnd, FColor::Blue, false, 0, 0, 1);
				DrawDebugSphere(GetWorld(), SecondaryHitResult.Location, 32.f, 16, FColor::Red, false, 0);
			}
#endif
			ShowReticle(SecondaryHitResult.Location);
			return SecondaryHitResult;
		}
	}
	HideReticle();
	InitialHitResult.bBlockingHit = false;
	return InitialHitResult;
}

bool AGameplayAbilityTargetActor_Teleport::HasEnoughSpace(const FVector& TargetLocation, AActor* InSourceActor) const
{
	const ACharacter* const Character = Cast<ACharacter>(InSourceActor);
	if (!Character)
	{
		return true;
	}

	const UCapsuleComponent* const CapsuleComponent = Character->GetCapsuleComponent();
	if (!CapsuleComponent)
	{
		return true;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AGameplayAbilityTargetActor_Teleport), false);

	const float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	const float CapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	const FCollisionShape Shape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	const FVector SweepLocation = TargetLocation + FVector(0,0, CapsuleHalfHeight + 0.1f);
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(InSourceActor);

	if (ReticleActor.IsValid())
	{
		ActorsToIgnore.Add(ReticleActor.Get());
	}

	Params.AddIgnoredActors(ActorsToIgnore);

	FHitResult SweepHitResult;
	const bool bHasHit = GetWorld()->SweepSingleByProfile(
		SweepHitResult,
		SweepLocation,
		SweepLocation,
		FQuat::Identity, 
		TraceProfile.Name, 
		Shape,
		Params
	);

#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugCapsule(GetWorld(), SweepLocation, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, bHasHit ? FColor::Red : FColor::Green, false, 0.0f);
	}
#endif
	
	return !bHasHit;
}

void AGameplayAbilityTargetActor_Teleport::PerformPrimaryTrace(FHitResult& OutHitResult, AActor* InSourceActor) const
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(InSourceActor);

	if (ReticleActor.IsValid())
	{
		ActorsToIgnore.Add(ReticleActor.Get());
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AGameplayAbilityTargetActor_Teleport), false);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	const FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	AimWithPlayerController(InSourceActor, Params, TraceStart, TraceEnd, false);

	LineTraceWithFilter(OutHitResult, GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);
}

void AGameplayAbilityTargetActor_Teleport::ShowReticle(const FVector& ReticleLocation)
{
	if (ReticleActor.IsValid())
	{
		ReticleActor->SetActorHiddenInGame(false);
		ReticleActor->SetIsTargetValid(true);
		ReticleActor->SetActorLocation(ReticleLocation);
	}
}

void AGameplayAbilityTargetActor_Teleport::HideReticle()
{
	if (ReticleActor.IsValid())
	{
		ReticleActor->SetActorHiddenInGame(true);
		ReticleActor->SetIsTargetValid(false);
	}
}
