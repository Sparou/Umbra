// Copyrighted by Vorona Games


#include "AbilitySystem/Interaction/Tasks/AbilityTask_UpdateInteractionTarget.h"

#include "AbilitySystemInterface.h"
#include "UmbraCollisionChannels.h"
#include "AbilitySystem/Abilities/UmbraBaseGameplayAbility.h"
#include "AbilitySystem/Interaction/InteractionStatics.h"
#include "Interface/OutlineInterface.h"
#include "Umbra/Umbra.h"

static int32 GCVarUmbraDrawDebugAbilityInteractionTrace = 0;
static FAutoConsoleVariableRef CVarUmbraDrawDebugInteractionTraceRef(
	TEXT("Umbra.DrawDebug.Ability.Interaction.Trace"),
	GCVarUmbraDrawDebugAbilityInteractionTrace,
	TEXT("[0] - Disable Interaction Trace visualization | [1] = Enable Interaction Trace visualization"),
	ECVF_Cheat
);

UAbilityTask_UpdateInteractionTarget::UAbilityTask_UpdateInteractionTarget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

UAbilityTask_UpdateInteractionTarget* UAbilityTask_UpdateInteractionTarget::UpdateInteractionTarget(
	UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate, FVector StartLocation)
{
	UAbilityTask_UpdateInteractionTarget* MyObj = NewAbilityTask<UAbilityTask_UpdateInteractionTarget>(OwningAbility);
	MyObj->InteractionScanRange = InteractionScanRange;
	MyObj->InteractionScanRate = InteractionScanRate;
	MyObj->StartLocation = StartLocation;
	return MyObj;
}

void UAbilityTask_UpdateInteractionTarget::Activate()
{
	SetWaitingOnAvatar();
	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::PerformTrace, InteractionScanRate, true);
}

void UAbilityTask_UpdateInteractionTarget::PerformTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!AvatarActor)
	{
		return;
	}

	StartLocation = AvatarActor->GetActorLocation();
	
	UWorld* World = GetWorld();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);

	const bool bTraceComplex = false;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_WaitForInteractableTargets_SingleLineTrace), bTraceComplex);
	Params.AddIgnoredActors(ActorsToIgnore);
	
	FVector TraceEnd;
	AimWithPlayerController(AvatarActor, Params, StartLocation, InteractionScanRange, OUT TraceEnd, true);

	FHitResult OutHitResult;
	LineTrace(OutHitResult, World, StartLocation, TraceEnd, ECC_Interaction, Params);

#if ENABLE_DRAW_DEBUG

	if (GCVarUmbraDrawDebugAbilityInteractionTrace)
	{
		DrawDebugLine(
			World,
			StartLocation,
			TraceEnd,
			FColor::Blue,
			false,
			InteractionScanRate);

		if (OutHitResult.bBlockingHit)
		{
			DrawDebugSphere(
				World,
				OutHitResult.Location,
				8.f,
				16,
				FColor::Blue,
				false,
				InteractionScanRate);
		}
	}
	
#endif
	
	if (!OutHitResult.bBlockingHit || !OutHitResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		HandleUnsuccessfulHit(OutHitResult);
	}
	else
	{
		HandleSuccessfulHit(OutHitResult);
	}
}

void UAbilityTask_UpdateInteractionTarget::LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End, ECollisionChannel Channel, FCollisionQueryParams Params)
{
	check(World);

	OutHitResult = FHitResult();
	TArray<FHitResult> HitResults;
	World->LineTraceMultiByChannel(HitResults, Start, End, Channel, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	if (HitResults.Num() > 0)
	{
		OutHitResult = HitResults[0];
	}
}

void UAbilityTask_UpdateInteractionTarget::AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd, bool bIgnoredPitch) const
{
	if (!Ability)
	{
		return;
	}

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	check(PC);

	FVector ViewStart;
	FRotator ViewRot;
	
	PC->GetPlayerViewPoint(ViewStart, ViewRot);
	FVector ViewDir = ViewRot.Vector();
	FVector ViewEnd = ViewStart + (ViewDir * MaxRange);
	ClipCameraRayToAbilityRange(ViewStart, ViewDir, TraceStart, MaxRange, ViewEnd);

	FHitResult HitResult;
	//LineTrace()

	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));
	const FVector AdjustedEnd = bUseTraceResult ? HitResult.Location : ViewEnd;
	FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
	
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = ViewDir;
	}

	if (bIgnoredPitch && bUseTraceResult)
	{
		FVector OriginalAimDir = (ViewEnd - TraceStart).GetSafeNormal();

		if (!OriginalAimDir.IsZero())
		{
			// Convert to angles and use original pitch
			const FRotator OriginalAimRot = OriginalAimDir.Rotation();

			FRotator AdjustedAimRot = AdjustedAimDir.Rotation();
			AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

			AdjustedAimDir = AdjustedAimRot.Vector();
		}
	}

	OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
}

bool UAbilityTask_UpdateInteractionTarget::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition)
{
	// Вычисляем вектор от камеры до центра сферы действия способности
	FVector CameraToCenter = AbilityCenter - CameraLocation;

	// Скалярное произведение вектора к центру и направления камеры
	// Показывает, насколько "прямо" камера смотрит на центр
	float DotToCenter = FVector::DotProduct(CameraToCenter, CameraDirection);

	// Если центр сферы находится перед камерой (угол между вектором и направлением <= 90°)
	if (DotToCenter >= 0)		
	{
		float DistanceSquared = CameraToCenter.SizeSquared() - (DotToCenter * DotToCenter);
		float RadiusSquared = (AbilityRange * AbilityRange);

		// Если луч проходит через сферу (перпендикулярное расстояние <= радиуса)
		if (DistanceSquared <= RadiusSquared)
		{
			// Вычисляем, на сколько единиц луч заходит внутрь сферы
			float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
			// Суммируем проекцию центра на луч и глубину захода внутрь сферы
			float DistanceAlongRay = DotToCenter + DistanceFromCamera;
			// Итоговая координата точки пересечения луча со сферой
			ClippedPosition = CameraLocation + (DistanceAlongRay * CameraDirection);		
			return true;
		}
	}
	// Если камера смотрит в противоположную сторону или луч мимо сферы,
	// возвращаем false и не меняем ClippedPosition
	return false;
}

void UAbilityTask_UpdateInteractionTarget::HandleSuccessfulHit(const FHitResult& HitResult)
{
	if (CurrentInteractionTarget != HitResult.GetActor())
	{
		UE_LOG(UmbraAbilitiesLog, Log, TEXT("New Interaction Target = [%s]"), *GetNameSafe(HitResult.GetActor()));

		if (HitResult.GetActor()->GetClass()->ImplementsInterface(UOutlineInterface::StaticClass()))
		{
			IOutlineInterface::Execute_EnableOutline(HitResult.GetActor(), NEUTRAL_OUTLINE_STENCIL_VALUE);
		}

		if (CurrentInteractionTarget.IsValid())
		{
			IOutlineInterface::Execute_DisableOutline(CurrentInteractionTarget.Get());
		}

		CurrentInteractionTarget = HitResult.GetActor();
		UpdateInteractionTargetDelegate.Broadcast(CurrentInteractionTarget.Get());
	}
}

void UAbilityTask_UpdateInteractionTarget::HandleUnsuccessfulHit(const FHitResult& HitResult)
{
	if (CurrentInteractionTarget.IsValid() && CurrentInteractionTarget.Get()->GetClass()->ImplementsInterface(UOutlineInterface::StaticClass()))
	{
		IOutlineInterface::Execute_DisableOutline(CurrentInteractionTarget.Get());
	}
	
	CurrentInteractionTarget = nullptr;
}

void UAbilityTask_UpdateInteractionTarget::OnDestroy(bool bInOwnerFinished)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TraceTimerHandle);
	}
	Super::OnDestroy(bInOwnerFinished);
}



