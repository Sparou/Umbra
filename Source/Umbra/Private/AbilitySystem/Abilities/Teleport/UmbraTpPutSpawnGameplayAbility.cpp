// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Teleport/UmbraTpPutSpawnGameplayAbility.h"

#include "Actors/UmbraPortalActor.h"
#include "GameFramework/Character.h"

FVector UUmbraTpPutSpawnGameplayAbility::LocationPortalStart = FVector::ZeroVector;
bool UUmbraTpPutSpawnGameplayAbility::bStartPlaced = false;

UUmbraTpPutSpawnGameplayAbility::UUmbraTpPutSpawnGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UUmbraTpPutSpawnGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	if (!bStartPlaced)
	{
		LocationPortalStart = AvatarActor->GetActorLocation();
		bStartPlaced = true;
		
		DrawDebugSphere(GetWorld(), LocationPortalStart, 30.f, 12, FColor::Blue, false, 5.f);
	}
	else
	{
		// Второй клик: создаем портал
		FVector LocationPortalEnd = AvatarActor->GetActorLocation();
		bStartPlaced = false;

		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
		FActorSpawnParameters Params;
		Params.Owner = Character;

		if (AUmbraPortalActor* PortalStart = GetWorld()->SpawnActor<AUmbraPortalActor>(PortalClass, LocationPortalStart, FRotator::ZeroRotator, Params))
		{
			if (AUmbraPortalActor* PortalEnd = GetWorld()->SpawnActor<AUmbraPortalActor>(PortalClass, LocationPortalEnd, FRotator::ZeroRotator, Params))
			{
				PortalStart->Init(LocationPortalEnd);
				PortalEnd->Init(LocationPortalStart);

				// Визуализация
				DrawDebugSphere(GetWorld(), LocationPortalEnd, 30.f, 12, FColor::Green, false, 5.f);
				DrawDebugSphere(GetWorld(), LocationPortalStart, 30.f, 12, FColor::Green, false, 5.f);
				DrawDebugLine(GetWorld(), LocationPortalStart, LocationPortalEnd, FColor::Cyan, false, 5.f, 0, 2.f);
				
			}
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
