// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Arrows/UmbraHitArrowGameplayAbility.h"
#include "Actor/UmbraArrowProjectile.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/UmbraPlayerController.h"


void UUmbraHitArrowGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
	CachedPlayerController = PC;

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
			FRotator LookAtRotation = (TargetPoint - OriginalPawn->GetActorLocation()).Rotation();
			OriginalPawn->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f)); // Только по Yaw, чтобы не наклонять вверх/вниз
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

void UUmbraHitArrowGameplayAbility::SpawnArrow(AUmbraPlayerController* PC, FVector SpawnLocation, FRotator SpawnRotation, FVector Direction) const
{
	if (AUmbraArrowProjectile* Arrow = PC->GetWorld()->SpawnActor<AUmbraArrowProjectile>(
				ArrowClass, SpawnLocation, SpawnRotation))
	{
		if (PC)
		{
			PC->SwitchToArrowContext();
			PC->Possess(Arrow);

			Arrow->Movement->Velocity = Arrow->GetActorForwardVector() * Arrow->Movement->InitialSpeed;


			Arrow->OnDestroyed.AddDynamic(this, &UUmbraHitArrowGameplayAbility::OnArrowDestroyed);
			Arrow->SetLifeSpan(AbilityDuration);
		}
	}
}

void UUmbraHitArrowGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UUmbraHitArrowGameplayAbility::OnArrowDestroyed(AActor* DestroyedActor)
{
	// Вернуть управление персонажу
	CachedPlayerController->SwitchToDefaultContext();
	CachedPlayerController->Possess(Cast<APawn>(OriginalPawn));
	
	if (DestroyedActor)
	{
		DestroyedActor->Destroy();
	}
}
