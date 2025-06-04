// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Projectile/UmbraProjectileGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UmbraGameplayTags.h"
#include "Actor/UmbraBaseProjectile.h"
#include "AI/UmbraAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/UmbraBaseCharacter.h"

void UUmbraProjectileGameplayAbility::SpawnProjectile(const FVector& SpawnLocation, bool bDrawDebug)
{
	if(!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	const FVector TargetLocation = GetTargetLocation();
	const FRotator SpawnRotation = (TargetLocation - SpawnLocation).Rotation();

	UE_LOG(UmbraAbilitiesLog, Log, TEXT("%s: Spawn Location = [%s]"), *GetNameSafe(this), *SpawnLocation.ToString());\
	UE_LOG(UmbraAbilitiesLog, Log, TEXT("%s: Spawn Rotation = [%s]"), *GetNameSafe(this), *SpawnRotation.ToString())
	UE_LOG(UmbraAbilitiesLog, Log, TEXT("%s: Target Location = [%s]"), *GetNameSafe(this), *TargetLocation.ToString());

	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), SpawnLocation, 5, 10, FColor::Green, false, 3.f);
		DrawDebugSphere(GetWorld(), TargetLocation, 5, 10, FColor::Red, false, 3.f);
		DrawDebugLine(GetWorld(), SpawnLocation, TargetLocation, FColor::Blue, false, 2.f);
	}
	
	FTransform Transform = FTransform();
	Transform.SetLocation(SpawnLocation);
	Transform.SetRotation(SpawnRotation.Quaternion());

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
	
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(GameplayEffectSpecHandle, FUmbraGameplayTags::Get().Combat_Damage, ScaledDamage);
	
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
