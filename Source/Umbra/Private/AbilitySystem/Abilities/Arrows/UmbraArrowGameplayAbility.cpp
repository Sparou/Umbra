// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Arrows/UmbraArrowGameplayAbility.h"

#include "MotionWarpingComponent.h"
#include "AbilitySystem/Abilities/GameplayAbilitiesFunctionLibrary.h"
#include "Actor/UmbraArrowProjectile.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/UmbraPlayerController.h"


void UUmbraArrowGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!ArrowClass || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Получаем PlayerController
	AUmbraPlayerController* PC = Cast<AUmbraPlayerController>(ActorInfo->PlayerController);
	if (!PC && ActorInfo->AvatarActor.IsValid())
	{
		PC = Cast<AUmbraPlayerController>(ActorInfo->AvatarActor->GetInstigatorController());
	}

	// Центр экрана
	int32 ViewportX, ViewportY;
	PC->GetViewportSize(ViewportX, ViewportY);
	FVector2D ScreenCenter(ViewportX / 2.f, ViewportY / 2.f);

	// Преобразуем экранные координаты в направление в мире
	FVector WorldLocation;
	FVector WorldDirection;

	if (PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
	{
		// Делаем LineTrace, чтобы узнать, куда целится игрок (например, в стену или воздух)
		FVector TraceStart = WorldLocation;
		FVector TraceEnd = TraceStart + WorldDirection * 10000.f;

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(ActorInfo->AvatarActor.Get());

		bool bHit = PC->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

		FVector TargetPoint = bHit ? HitResult.ImpactPoint : TraceEnd;
		OriginalPawn = ActorInfo->AvatarActor.Get();
		FVector Direction = (TargetPoint - OriginalPawn->GetActorLocation()).GetSafeNormal();
		FRotator SpawnRotation = Direction.Rotation();
		
		// Повернуть Pawn в сторону выстрела
		if (OriginalPawn)
		{
			FRotator LookAtRotation = UGameplayAbilitiesFunctionLibrary::GetRotationToTarget(GetAvatarActorFromActorInfo()->GetActorLocation(), TargetPoint);
			if (UMotionWarpingComponent* MotionWarping = OriginalPawn->FindComponentByClass<UMotionWarpingComponent>())
			{
				MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("RotationWarp", FVector::ZeroVector, LookAtRotation);
			}
		}

		const FVector SpawnLocation = OriginalPawn->GetActorLocation() + OriginalPawn->GetActorForwardVector() * ForwardOffset + FVector::UpVector * HeightOffset;

		FTimerDelegate TimerDel;
		TimerDel.BindLambda([this, PC, SpawnLocation, SpawnRotation, Direction]()
		{
			SpawnArrow(PC, SpawnLocation, SpawnRotation, Direction);
		});


		GetWorld()->GetTimerManager().SetTimer(
			SpawnArrowTimerHandle,
			TimerDel,
			DurationAnimMontage,
			false
		);
	}
}

void UUmbraArrowGameplayAbility::SpawnArrow(AUmbraPlayerController* PC, FVector SpawnLocation, FRotator SpawnRotation, FVector Direction) const
{	
	// Спавн снаряда
	if (AUmbraArrowProjectile* Arrow = PC->GetWorld()->SpawnActor<AUmbraArrowProjectile>(
		ArrowClass, SpawnLocation, SpawnRotation))
	{
		Arrow->Movement->Velocity = Direction * Arrow->Movement->InitialSpeed;
	}
	
}

