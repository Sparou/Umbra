// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Projectile/UmbraProjectileGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/UmbraBaseProjectile.h"
#include "AI/UmbraAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/UmbraBaseCharacter.h"

void UUmbraProjectileGameplayAbility::SpawnProjectile(const FVector& SpawnLocation)
{
	if(!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	const FVector TargetLocation = GetTargetLocation();
	const FRotator SpawnRotation = (TargetLocation - SpawnLocation).Rotation();
	FTransform Transform = FTransform();
	Transform.SetLocation(SpawnLocation);
	Transform.SetRotation(SpawnRotation.Quaternion());

	UE_LOG(UmbraAbilitiesLog, Warning, TEXT("Spawn location = [%s]"), *SpawnLocation.ToString());
	UE_LOG(UmbraAbilitiesLog, Warning, TEXT("Target location = [%s]"), *TargetLocation.ToString());
	//DrawDebugLine(GetWorld(), SpawnLocation, TargetLocation, FColor::Red, true);
	//DrawDebugSphere(GetWorld(), SpawnLocation, 5.f, 16, FColor::Red, true);

	AUmbraBaseProjectile* Projectile = GetWorld()->SpawnActorDeferred<AUmbraBaseProjectile>(
		ProjectileClass,
		Transform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()); 
	const FGameplayEffectSpecHandle GameplayEffectSpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffectClass,
		GetAbilityLevel(),
		SourceASC->MakeEffectContext());
	Projectile->SetGameplayEffectSpecHandle(GameplayEffectSpecHandle);

	Projectile->FinishSpawning(Transform);
}

FVector UUmbraProjectileGameplayAbility::GetTargetLocation()
{
	const AUmbraBaseCharacter* BaseCharacter = Cast<AUmbraBaseCharacter>(GetAvatarActorFromActorInfo());
	if(!BaseCharacter)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[%s] isn't UmbraBaseCharacter!"), *BaseCharacter->GetName());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	if(BaseCharacter->IsPlayerControlled())
	{
		FVector TargetLocation;
		FRotator TargetRotation;
		BaseCharacter->GetController()->GetPlayerViewPoint(TargetLocation, TargetRotation);

		const FVector ViewDirection = TargetRotation.Vector();
		return TargetLocation + ViewDirection * TraceDistance;
	}

	//AI controlled pawn logic
	AUmbraAIController* AIController = Cast<AUmbraAIController>(BaseCharacter->GetController());
	if(!AIController)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[%s] isn't controlled by UmbraAIController!"), *BaseCharacter->GetName());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	const UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if(!BlackboardComponent)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[%s] has no BlackboardComponent!"), *BaseCharacter->GetName());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	const ACharacter* TargetCharacter = Cast<ACharacter>(BlackboardComponent->GetValueAsObject(BlackboardVariableName));
	if(!TargetCharacter)
	{
		UE_LOG(UmbraAbilitiesLog, Warning, TEXT("[%s] has no CurrentEnemy in Blackboard!"), *BaseCharacter->GetName());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	return TargetCharacter->GetMesh()->GetBoneLocation(BoneName);
}
