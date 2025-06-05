// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Teleport/UmbraTpPutSpawnGameplayAbility.h"

#include "Actor/UmbraPortalActor.h"
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
	if (!IsLocallyControlled() || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!bStartPlaced)
	{
		LocationPortalStart = AvatarActor->GetActorLocation();
		bStartPlaced = true;
	}
	else
	{
		// Второй клик: создаем порталБ
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
			}
		}
	}	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
