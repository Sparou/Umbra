// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Trap/UmbraBaseTrapAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "Actor/UmbraEffectActor.h"

void UUmbraBaseTrapAbility::TryToPlaceTrap()
{
	WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this,
		NAME_None,
		EGameplayTargetingConfirmation::Type::UserConfirmed,
		AGameplayAbilityTargetActor_GroundTrace::StaticClass());

	if (WaitTargetDataTask == nullptr)
	{
		UE_LOG(UmbraAbilitiesLog, Error, TEXT("Failed to create WaitTargetDataTask. Ending ability."));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false); 
		return;
	}
	
	AGameplayAbilityTargetActor* TargetActor = nullptr;

	WaitTargetDataTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_GroundTrace::StaticClass(), TargetActor);

	AGameplayAbilityTargetActor_GroundTrace* TargetActorGroundTrace = Cast<AGameplayAbilityTargetActor_GroundTrace>(TargetActor);
	TargetActorGroundTrace->StartLocation = MakeTargetLocationInfoFromOwnerActor();
	TargetActorGroundTrace->MaxRange = Distance;
	TargetActorGroundTrace->bTraceAffectsAimPitch = true;
	TargetActorGroundTrace->ReticleClass = ReticleClass;
	TargetActorGroundTrace->bDebug = bDebugTargeting;
	TargetActorGroundTrace->CollisionRadius = TrapRadius;
	TargetActorGroundTrace->CollisionHeight = TrapHeight;
	TargetActorGroundTrace->TraceProfile = FCollisionProfileName("BlockAll");

	WaitTargetDataTask->FinishSpawningActor(this, TargetActor);
	WaitTargetDataTask->ValidData.AddDynamic(this, &UUmbraBaseTrapAbility::ConfirmTrapPlacement);
	WaitTargetDataTask->Cancelled.AddDynamic(this, &UUmbraBaseTrapAbility::CancelTrapPlacement);

	WaitTargetDataTask->Activate();
	
}

void UUmbraBaseTrapAbility::ConfirmTrapPlacement(const FGameplayAbilityTargetDataHandle& Data) 
{
	if (!WaitTargetDataTask)
	{
		WaitTargetDataTask->EndTask();
	}
	
	if (Data.Get(0)->GetHitResult()->bBlockingHit == false)
	{
		return;
	}

	FVector SpawnLocation = Data.Get(0)->GetHitResult()->Location + FVector(0, 0, TrapHeight);
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = GetAvatarActorFromActorInfo()->GetOwner();

	if (AUmbraEffectActor* SpawnedActor = GetWorld()->SpawnActor<AUmbraEffectActor>(TrapClass, SpawnTransform, SpawnParams))
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully spawned trap actor [%s] at %s."), *GetNameSafe(SpawnedActor), *SpawnLocation.ToString());
		OnTrapPlaced.Broadcast(SpawnedActor);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn trap actor from class [%s]."), *GetNameSafe(TrapClass));
		OnTrapPlacementCancelled.Broadcast(Data);
	}
}

void UUmbraBaseTrapAbility::CancelTrapPlacement(const FGameplayAbilityTargetDataHandle& Data) 
{
	if (!WaitTargetDataTask)
	{
		WaitTargetDataTask->EndTask();
	}

	OnTrapPlacementCancelled.Broadcast(Data);
}
