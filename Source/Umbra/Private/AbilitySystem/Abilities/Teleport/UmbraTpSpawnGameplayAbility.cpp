// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Teleport/UmbraTpSpawnGameplayAbility.h"
#include "Actor/UmbraPortalActor.h"
#include "GameFramework/Character.h"

void UUmbraTpSpawnGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
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
		End += Forward * MaxTeleportDistance;
		UE_LOG(LogTemp, Warning, TEXT("Teleport target: %s"), *Forward.ToString());
		
		
		bool bSafe = !GetWorld()->OverlapBlockingTestByChannel(
			End,
			FQuat::Identity,
			TraceChannel,
			FCollisionShape::MakeCapsule(34.f, 88.f), // Размер капсулы как у персонажа
			TraceParams
		);

		while (!bSafe)
		{
			End += Forward * WallOffset;
			// UE_LOG(LogTemp, Warning, TEXT("Teleport target: %s"), *Forward.ToString());
			
			bSafe = !GetWorld()->OverlapBlockingTestByChannel(End, FQuat::Identity, TraceChannel, FCollisionShape::MakeCapsule(34.f, 88.f), TraceParams);
		}
		AvatarActor->SetActorLocation(End);
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("No hit"));
		AvatarActor->SetActorLocation(End);
	}
	// DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.0f, 0, 2.0f);
	
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	FActorSpawnParameters Params;
	Params.Owner = Character;
	if (AUmbraPortalActor* Portal = GetWorld()->SpawnActor<AUmbraPortalActor>(PortalClass, Start, FRotator::ZeroRotator, Params))
	{
		Portal->Init(End);
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
