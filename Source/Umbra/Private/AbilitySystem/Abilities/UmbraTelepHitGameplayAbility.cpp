// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/UmbraTelepHitGameplayAbility.h"

void UUmbraTelepHitGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	InitialLocation = AvatarActor->GetActorLocation();
	FVector Forward = AvatarActor->GetActorForwardVector();
	FVector End = InitialLocation + Forward * MaxTeleportDistance;

	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(AvatarActor);

	DrawDebugLine(GetWorld(), InitialLocation, End, FColor::Green, false, 2.f, 0, 1.f);
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult,InitialLocation, End, TraceChannel, TraceParams))
	{
		FVector TeleportTarget = End + Forward * MaxTeleportDistance;
		UE_LOG(LogTemp, Warning, TEXT("Teleport target: %s"), *Forward.ToString());
		
		bool bSafe = !GetWorld()->OverlapBlockingTestByChannel(
			TeleportTarget,
			FQuat::Identity,
			TraceChannel,
			FCollisionShape::MakeCapsule(34.f, 88.f), // Размер капсулы как у персонажа
			TraceParams
		);

		while (!bSafe)
		{
			TeleportTarget += Forward * WallOffset;
			UE_LOG(LogTemp, Warning, TEXT("Teleport target: %s"), *Forward.ToString());
			bSafe = !GetWorld()->OverlapBlockingTestByChannel(TeleportTarget, FQuat::Identity, TraceChannel, FCollisionShape::MakeCapsule(34.f, 88.f), TraceParams);
		}
		AvatarActor->SetActorLocation(TeleportTarget);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No hit"));
		AvatarActor->SetActorLocation(End);
	}

	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TeleportBackTimerHandle,
			FTimerDelegate::CreateUObject(
				this,
				&UUmbraTelepHitGameplayAbility::TeleportBackAndEnd,
				Handle,
				ActorInfo,
				ActivationInfo
			),
			DurationTelep,
			false
		);
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UUmbraTelepHitGameplayAbility::TeleportBackAndEnd(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		AActor* AvatarActor = ActorInfo->AvatarActor.Get();
		AvatarActor->SetActorLocation(InitialLocation);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}