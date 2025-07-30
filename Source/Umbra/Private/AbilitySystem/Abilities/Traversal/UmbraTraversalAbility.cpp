// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Traversal/UmbraTraversalAbility.h"

#include "UmbraGameplayTags.h"
#include "Character/UmbraBaseCharacter.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "MotionWarpingComponent.h"
#include "Character/Data/TraversalActionsData.h"

AUmbraBaseCharacter* UUmbraTraversalAbility::GetUmbraCharacter() const
{
	if (!UmbraCharacter.IsValid())
	{
		return nullptr;
	}

	return UmbraCharacter.Get();
}

UCharacterMovementComponent* UUmbraTraversalAbility::GetCharacterMovementComponent() const
{
	if (!Movement.IsValid())
	{
		return nullptr;
	}

	return Movement.Get();
}

UCapsuleComponent* UUmbraTraversalAbility::GetCapsuleComponent() const
{
	if (!Capsule.IsValid())
	{
		return nullptr;
	}

	return Capsule.Get();
}

UMotionWarpingComponent* UUmbraTraversalAbility::GetMotionWarpingComponent() const
{
	if (!MotionWarping.IsValid())
	{
		return nullptr;
	}

	return MotionWarping.Get();
}

UTraversalActionsData* UUmbraTraversalAbility::GetTraversalActions() const
{
	if (!TraversalActions.IsValid())
	{
		return nullptr;
	}

	return TraversalActions.Get();
}

bool UUmbraTraversalAbility::DetectObstacle(FGameplayTag& ObstacleTag, float DetectionDistance, const FVector& BoxHalfSize, bool bDrawDebug)
{
	ObstacleTag = FGameplayTag();
	
	UWorld* World = GetWorld();
	AActor* Avatar = GetAvatarActorFromActorInfo();

	if (!World || !Avatar)
	{
		return false;
	}

	const FVector OverlapLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * DetectionDistance;
	const FCollisionShape Shape = FCollisionShape::MakeBox(BoxHalfSize);

	TArray<FOverlapResult> Overlaps;
	const bool bHasOverlap = World->OverlapMultiByChannel(
		Overlaps,
		OverlapLocation,
		Avatar->GetActorQuat(),
		ECC_Visibility,
		Shape);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugBox(GetWorld(), OverlapLocation, BoxHalfSize, Avatar->GetActorQuat(), FColor::Yellow, false, 3.f);
	}

#endif

	FUmbraGameplayTags UGT = FUmbraGameplayTags::Get();
	
	for (const FOverlapResult& OverlapResult : Overlaps)
	{
		if (const AActor* const OverlapActor = OverlapResult.GetActor())
		{
			if (OverlapActor->ActorHasTag(UGT.Ability_Movement_Traversal_Climb.GetTagLeafName()))
			{
				ObstacleTag = UGT.Ability_Movement_Traversal_Climb;
			}
			else if (OverlapActor->ActorHasTag(UGT.Ability_Movement_Traversal_Mantle.GetTagLeafName()))
			{
				ObstacleTag = UGT.Ability_Movement_Traversal_Mantle;
			}
			else if (OverlapActor->ActorHasTag(UGT.Ability_Movement_Traversal_Vault.GetTagLeafName()))
			{
				ObstacleTag = UGT.Ability_Movement_Traversal_Vault;
			}
			
			if (ObstacleTag.IsValid())
			{
				return true;
			}
		}
	}
	
	return false;
}

bool UUmbraTraversalAbility::FindObstacleHitResult(FHitResult& ObstacleHitResult, float InitialDownOffset, const int32 Iterations, float DetectionDistance, float OffsetStep, bool bDrawDebug)
{
	ObstacleHitResult = FHitResult();
	FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector InitialPosition = AvatarLocation + FVector::DownVector * InitialDownOffset;
	
	for (int32 i = 0; i < Iterations; ++i)
	{
		FVector TraceStart = InitialPosition + FVector::UpVector * OffsetStep * i;
		FVector TraceEnd = TraceStart + GetAvatarActorFromActorInfo()->GetActorForwardVector() * DetectionDistance;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetAvatarActorFromActorInfo());
		
		bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(ObstacleHitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

#if ENABLE_DRAW_DEBUG
		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), ObstacleHitResult.TraceStart, ObstacleHitResult.TraceEnd, FColor::Green, false, 3.f);
			if (bBlockingHit)
			{
				DrawDebugSphere(GetWorld(), ObstacleHitResult.ImpactPoint, 4, 4, FColor::Green, false, 3.f);
			}
		}
#endif

		if (bBlockingHit)
		{
			return true;
		}
	}

	return false;
}

bool UUmbraTraversalAbility::FindObstacleEdgeResult(FHitResult& EdgeResult, const FHitResult& ObstacleHitResult, int32 Iterations,  float OffsetStep, float DetectionDistance, float Threshold, bool bDrawDebug)
{
	EdgeResult = FHitResult();
	TArray<FHitResult> HitResults;
	HitResults.Reserve(Iterations);
	TArray<FHitResult> EdgeResults;
	
	for (int32 i = 0; i < Iterations; ++i)
	{
		FVector TraceStart = ObstacleHitResult.ImpactPoint + FVector::UpVector * OffsetStep * i;
		FVector TraceEnd = TraceStart + GetAvatarActorFromActorInfo()->GetActorForwardVector() * DetectionDistance;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetAvatarActorFromActorInfo());
		FHitResult HitResult;
		
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

#if ENABLE_DRAW_DEBUG
		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Orange, false, 3.f);
		}
#endif
		HitResults.Add(HitResult);
	}

	for (int i = 1; i < HitResults.Num(); ++i)
	{
		FHitResult CurrentHitResult = HitResults[i];
		float CurrentDistance = CurrentHitResult.bBlockingHit ? CurrentHitResult.Distance : FVector::DistSquared(CurrentHitResult.TraceStart, CurrentHitResult.TraceEnd);
		FHitResult PreviousHitResult = HitResults[i - 1];
		float PreviousDistance = PreviousHitResult.bBlockingHit ? PreviousHitResult.Distance : FVector::DistSquared(PreviousHitResult.TraceStart, PreviousHitResult.TraceEnd);

		if (CurrentDistance - PreviousDistance > 5)
		{
			EdgeResults.Add(PreviousHitResult);
		}
	}

	if (EdgeResults.Num() <= 0)
	{
		return false;
	}

	EdgeResult = EdgeResults[0];
	

	for (int32 i = 1; i < EdgeResults.Num(); ++i)
	{
		float NewDistance = FVector::DistSquared(GetAvatarActorFromActorInfo()->GetActorLocation(), EdgeResults[i].ImpactPoint);
		float CurrentDistance = FVector::DistSquared(GetAvatarActorFromActorInfo()->GetActorLocation(), EdgeResult.ImpactPoint);
		if (NewDistance < CurrentDistance) EdgeResult = EdgeResults[i];
	}

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), EdgeResult.ImpactPoint, 4, 4, FColor::Orange, false, 3.f);
	}
#endif

	return true;
}

bool UUmbraTraversalAbility::FindObstacleTopResult(FHitResult& TopHitResult, const FHitResult& EdgeHitResult, int32 Iterations, float TraceHeight, float OffsetStep, bool bDrawDebug)
{
	TopHitResult = FHitResult();
	for (int i = 0; i < Iterations; ++i)
	{
		FVector TraceStart = EdgeHitResult.ImpactPoint + FVector::UpVector * TraceHeight + -EdgeHitResult.ImpactNormal * OffsetStep * i;
		FVector TraceEnd = TraceStart + FVector::DownVector * TraceHeight;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetAvatarActorFromActorInfo());
		FHitResult HitResult;
		
		bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(TopHitResult, TraceStart, TraceEnd, ECC_Visibility, Params);
		
#if ENABLE_DRAW_DEBUG
		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::White, false, 3.f);
			if (bBlockingHit)
			{
				DrawDebugSphere(GetWorld(), TopHitResult.ImpactPoint, 4, 4, FColor::White, false, 3.f);
			}
		}
#endif

		if (bBlockingHit)
		{
			UE_LOG(UmbraAbilitiesLog, Log, TEXT("Obstacle top surface found for [%s]"), *GetNameSafe(GetAvatarActorFromActorInfo()));
			return true;
		}
	}

	return false;
}

bool UUmbraTraversalAbility::FindObstacleDepthResult(FHitResult& DepthHitResult, const FHitResult& TopHitResult, const FVector& Direction, int32 Iterations, float OffsetStep, float Threshold, bool bDrawDebug)
{
	DepthHitResult = FHitResult();
	bool bBlockingHit = false;
	
	for (int32 i = 0; i < Iterations; ++i)
	{
		FVector TraceStart = TopHitResult.ImpactPoint + Direction * OffsetStep * i;
		FVector TraceEnd = TraceStart + FVector::DownVector * Threshold;
		FHitResult HitResult;

		bBlockingHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility);

#if ENABLE_DRAW_DEBUG
		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Black, false, 3.f);
		}
#endif

		if (!bBlockingHit)
		{
			UE_LOG(UmbraAbilitiesLog, Log, TEXT("End of wall found for [%s]"), *GetNameSafe(GetAvatarActorFromActorInfo()));
			break;
		}

		DepthHitResult = HitResult;
	}

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), DepthHitResult.ImpactPoint, 4, 4, FColor::Black, false, 3.f);
	}
#endif

	return DepthHitResult.bBlockingHit && !bBlockingHit;
}

bool UUmbraTraversalAbility::FindVaultResult(FHitResult& VaultResult, const FHitResult& DepthResult, float Distance, float MaxHeight, bool bDrawDebug)
{
	VaultResult = FHitResult();
	FVector TraceStart = DepthResult.ImpactPoint + GetAvatarActorFromActorInfo()->GetActorForwardVector() * Distance;
	FVector TraceEnd = TraceStart + FVector::DownVector * MaxHeight;

	bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(VaultResult, TraceStart, TraceEnd, ECC_Visibility);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 3.f);
		if (bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), VaultResult.ImpactPoint, 4, 4, FColor::Blue, false, 3.f);
		}
	}
#endif

	return bBlockingHit;
}

bool UUmbraTraversalAbility::HasEnoughSpace(float CapsuleHalfHeight, float CapsuleHalfRadius, const FVector& OverlapLocation, const TArray<AActor*>& ActorsToIgnore, bool bDrawDebug)
{
	const FVector LiftedOverlapLocation = OverlapLocation + FVector(0, 0, CapsuleHalfHeight + 10.f);

	FCollisionShape Shape = FCollisionShape::MakeCapsule(CapsuleHalfRadius, CapsuleHalfHeight);
	FCollisionQueryParams Params;
	Params.AddIgnoredActors(ActorsToIgnore);
		
	bool bOverlap = GetWorld()->OverlapAnyTestByChannel(LiftedOverlapLocation, FQuat::Identity, ECC_Visibility, Shape);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		if (bOverlap)
		{
			DrawDebugCapsule(GetWorld(), LiftedOverlapLocation, CapsuleHalfHeight, CapsuleHalfRadius * 2, FQuat::Identity, FColor::Red, false, 3.f);
		}
		else
		{
			DrawDebugCapsule(GetWorld(), LiftedOverlapLocation, CapsuleHalfHeight, CapsuleHalfRadius * 2, FQuat::Identity, FColor::Green, false, 3.f);
		}
	}
#endif

	return !bOverlap;
}

void UUmbraTraversalAbility::FindWarpLocation(FVector& WarpLocation, const FVector& InitialLocation, const FRotator& Rotation, float XOffset, float YOffset, float ZOffset, bool bDrawDebug)
{
	FUmbraGameplayTags UGT = FUmbraGameplayTags::Get();
	FVector X = MoveVector(InitialLocation, XOffset, UGT.Direction_X_Forward, Rotation);
	FVector Y = MoveVector(X, YOffset, UGT.Direction_Y_Right, Rotation);
	FVector Z = MoveVector(Y, ZOffset, UGT.Direction_Z_Up);

	WarpLocation = Z;
	
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), WarpLocation, 4, 4, FColor::Blue, false, 3.f);
	}
#endif
	UE_LOG(UmbraAbilitiesLog, Log, TEXT("Motion Warping Location = [%s]"), *WarpLocation.ToString());
}

void UUmbraTraversalAbility::SetupMotionWarping(const FVector& TopImpactPoint, const FVector& DepthImpactPoint, const FVector& VaultImpactPoint, const FRotator& TargetRotation, bool bDrawDebug)
{
	if (TraversalActionMontage.MotionWarpingParameters == 0)
	{
		return;
	}

	FVector TopWarpLocation = FVector::ZeroVector;
	FindWarpLocation(
		TopWarpLocation,
		TopImpactPoint,
		TargetRotation,
		TraversalActionMontage.TopWarpOffset.X,
		TraversalActionMontage.TopWarpOffset.Y,
		TraversalActionMontage.TopWarpOffset.Z,
		bDrawDebug
	);

	UMotionWarpingComponent* MotionWarpingComponent = GetMotionWarpingComponent();
	
	if (!MotionWarpingComponent)
	{
		UE_LOG(UmbraAbilitiesLog, Error, TEXT("Motion Warping is null in [%s]"), *GetNameSafe(this));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("TopWarp"), TopWarpLocation, TargetRotation);

	if (TraversalActionMontage.MotionWarpingParameters == 1)
	{
		return;
	}

	FVector BalanceWarpLocation = FVector::ZeroVector;
	FindWarpLocation(
		BalanceWarpLocation,
		TopImpactPoint,
		TargetRotation,
		TraversalActionMontage.BalanceWarpOffset.X,
		TraversalActionMontage.BalanceWarpOffset.Y,
		TraversalActionMontage.BalanceWarpOffset.Z,
		bDrawDebug
	);

	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("BalanceWarp"), BalanceWarpLocation, TargetRotation);

	if (TraversalActionMontage.MotionWarpingParameters == 2)
	{
		return;
	}

	FVector DepthWarpLocation = FVector::ZeroVector;
	FindWarpLocation(
		DepthWarpLocation,
		DepthImpactPoint,
		TargetRotation,
		TraversalActionMontage.DepthWarpOffset.X,
		TraversalActionMontage.DepthWarpOffset.Y,
		TraversalActionMontage.DepthWarpOffset.Z,
		bDrawDebug
	);

	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("DepthWarp"), DepthWarpLocation, TargetRotation);

	if (TraversalActionMontage.MotionWarpingParameters == 3)
	{
		return;
	}

	FVector VaultWarpLocation = FVector::ZeroVector;
	FindWarpLocation(
		VaultWarpLocation,
		VaultImpactPoint,
		TargetRotation,
		TraversalActionMontage.VaultWarpOffset.X,
		TraversalActionMontage.VaultWarpOffset.Y,
		TraversalActionMontage.VaultWarpOffset.Z,
		bDrawDebug
	);

	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("VaultWarp"), VaultWarpLocation, TargetRotation);
}

void UUmbraTraversalAbility::ApplyTraversalSettings()
{
	FTraversalAction TraversalAction;

	if (!GetTraversalActions()->FindActionByTag(TraversalAction, TraversalActionTag))
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("Cannot apply traversal setting in [%s], traversal action tag is none or cannot find traversal action associated with this tag"), *GetNameSafe(this));
		return;
	}

	if (Capsule.IsValid())
	{
		Capsule->SetCollisionEnabled(TraversalAction.CollisionType);
	}

	if (Movement.IsValid())
	{
		Movement->SetMovementMode(TraversalAction.MovementMode);
	}
}

void UUmbraTraversalAbility::RestoreTraversalSettings()
{
	if (Capsule.IsValid())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}

	if (Movement.IsValid())
	{
		Movement->SetMovementMode(MOVE_Walking);
	}
}

void UUmbraTraversalAbility::ReverseNormal(FRotator& ReversedNormal, const FVector& Normal)
{
	FRotator Rotator = UKismetMathLibrary::MakeRotFromX(Normal);
	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(Rotator, FRotator(0.f, 180.f, 0.f ));
	ReversedNormal = FRotator(0.f, DeltaRotator.Yaw, 0.f);
}

void UUmbraTraversalAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	UmbraCharacter = Cast<AUmbraBaseCharacter>(ActorInfo->AvatarActor);

	if (!UmbraCharacter.IsValid())
	{
		UE_LOG(UmbraAbilitiesLog, Error, TEXT("Character is null at [%s]"), *GetNameSafe(this));
		return;
	}

	Movement = UmbraCharacter->GetCharacterMovement();

	if (!Movement.IsValid())
	{
		UE_LOG(UmbraAbilitiesLog, Error, TEXT("Movement Component is null at [%s]"), *GetNameSafe(this));
	}

	Capsule = UmbraCharacter->GetCapsuleComponent();

	if (!Capsule.IsValid())
	{
		UE_LOG(UmbraAbilitiesLog, Error, TEXT("Capsule Component is null at [%s]"), *GetNameSafe(this));
	}

	MotionWarping = UmbraCharacter->GetMotionWarpingComponent();
	if (!MotionWarping.IsValid())
	{
		UE_LOG(UmbraAbilitiesLog, Error, TEXT("Motion Warping Component is null at [%s]"), *GetNameSafe(this));
	}

	TraversalActions = UmbraCharacter->GetTraversalActionsData();
	if (!TraversalActions.IsValid())
	{
		UE_LOG(UmbraAbilitiesLog, Error, TEXT("Traversal Actions is null at [%s]"), *GetNameSafe(this));
	}
}

FVector UUmbraTraversalAbility::MoveVector(const FVector& Source, float Offset, const FGameplayTag& DirectionTag, const FRotator& Rotator) const
{
	FUmbraGameplayTags UGT = FUmbraGameplayTags::Get();
	FRotationMatrix RotationMatrix = Rotator == FRotator::ZeroRotator ? FRotationMatrix(GetAvatarActorFromActorInfo()->GetActorRotation()) : FRotationMatrix(Rotator);

	if (DirectionTag.MatchesTagExact(UGT.Direction_X_Forward))
	{
		return Source + Offset * RotationMatrix.GetScaledAxis(EAxis::X);
	}
	if (DirectionTag.MatchesTagExact(UGT.Direction_X_Backward))
	{
		return Source - Offset * RotationMatrix.GetScaledAxis(EAxis::X);
	}
	if (DirectionTag.MatchesTagExact(UGT.Direction_Y_Right))
	{
		return Source + Offset * RotationMatrix.GetScaledAxis(EAxis::Y);
	}
	if (DirectionTag.MatchesTagExact(UGT.Direction_Y_Left))
	{
		return Source - Offset * RotationMatrix.GetScaledAxis(EAxis::Y);
	}
	if (DirectionTag.MatchesTagExact(UGT.Direction_Z_Up))
	{
		return Source + FVector(0, 0, Offset);
	}
	if (DirectionTag.MatchesTagExact(UGT.Direction_Z_Down))
	{
		return Source - FVector(0, 0, Offset);
	}

	UE_LOG(UmbraAbilitiesLog, Error, TEXT("Invalid direction tag in MoveVector call from [%s]"), *GetNameSafe(this));
	return Source;
}



