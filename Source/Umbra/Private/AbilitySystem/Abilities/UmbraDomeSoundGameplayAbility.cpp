// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/UmbraDomeSoundGameplayAbility.h"

#include "Actors/UmbraSoundDome.h"


void UUmbraDomeSoundGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor || !DomeActorClass) return;

	FVector SpawnLocation = AvatarActor->GetActorLocation();
	FActorSpawnParameters Params;
	Params.Owner = AvatarActor;
	Params.Instigator = Cast<APawn>(AvatarActor);

	if (AUmbraSoundDome* SpawnedDome = AvatarActor->GetWorld()->SpawnActor<AUmbraSoundDome>(DomeActorClass, SpawnLocation, FRotator::ZeroRotator, Params))
	{
		UE_LOG(LogTemp, Display, TEXT("Init Dome"));
		SpawnedDome->Init(DomeRadius, AbilityDuration);
	}
}

void UUmbraDomeSoundGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
