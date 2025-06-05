// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Teleport/UmbraTpFdCooldownGameplayAbility.h"



void UUmbraTpFdCooldownGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	FVector Start = AvatarActor->GetActorLocation();
	FVector Forward = AvatarActor->GetActorForwardVector();
	FVector End = Start + Forward * MaxTeleportDistance;

	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(AvatarActor);
	
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,Start, End, TraceChannel, TraceParams);
	
	if (bHit)
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
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
