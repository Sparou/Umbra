// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Arrows/UmbraHitArrowGameplayAbility.h"
#include "Actors/UmbraArrowProjectile.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/UmbraPlayerController.h"


void UUmbraHitArrowGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
		TEXT("Hit Arrow Ability Activated!"));
		
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
		TEXT("Hit Arrow Ability Activated!"));
	
	if (!ArrowClass || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FGameplayAbilityActorInfo* LocalActorInfo = ActorInfo;

	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this, LocalActorInfo]()
	{
		if (LocalActorInfo && LocalActorInfo->AvatarActor.IsValid())
		{
			SpawnArrow(LocalActorInfo);
		}
	});

	GetWorld()->GetTimerManager().SetTimer(
		SpawnArrowTimerHandle,
		TimerDel,
		DurationAnimMontage,
		false
	);
}

void UUmbraHitArrowGameplayAbility::SpawnArrow(const FGameplayAbilityActorInfo* ActorInfo)
{
	OriginalPawn = ActorInfo->AvatarActor.Get();
	AUmbraPlayerController* PC = Cast<AUmbraPlayerController>(ActorInfo->PlayerController);
	if (!PC && ActorInfo->AvatarActor.IsValid())
	{
		PC = Cast<AUmbraPlayerController>(ActorInfo->AvatarActor->GetInstigatorController());
	}
	CachedPlayerController = PC;

	FVector SpawnLocation = OriginalPawn->GetActorLocation() + OriginalPawn->GetActorForwardVector() * 500.f;
	FRotator SpawnRotation = OriginalPawn->GetActorRotation();
	

	if (AUmbraArrowProjectile* Arrow = ActorInfo->AvatarActor->GetWorld()->SpawnActor<AUmbraArrowProjectile>(
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
