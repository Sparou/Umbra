// Copyrighted by Vorona Games

#include "Character/UmbraPlayerCharacter.h"
#include "Character/Component/TraversalComponent.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/UmbraAttributeSet.h"
#include "Character/Component/StealthComponent.h"
#include "Player/UmbraPlayerState.h"
#include "Umbra/Umbra.h"

AUmbraPlayerCharacter::AUmbraPlayerCharacter(const FObjectInitializer& ObjInit)
{
	//TraversalComponent = CreateDefaultSubobject<UTraversalComponent>("Traversal Component");
	StealthComponent = CreateDefaultSubobject<UStealthComponent>("Stealth Component");
}

UAssassinationsData* AUmbraPlayerCharacter::GetAssassinationsData()
{
	return AssassinationsData;
}


UAISense_Sight::EVisibilityResult AUmbraPlayerCharacter::CanBeSeenFrom(const FCanBeSeenFromContext& Context,
	FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested,
	float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Context.IgnoreActor);
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Context.ObserverLocation,
		this->GetActorLocation(),
		ECC_Visibility,
		CollisionParams
	);

	// if (bHit)
	// {
	// 	DrawDebugLine(GetWorld(),
	// 		Context.ObserverLocation,
	// 		HitResult.ImpactPoint,
	// 		FColor::Green,
	// 		false,
	// 		5.f,
	// 		0,
	// 		1.0f);
	// }

	AActor* HitActor = Cast<AActor>(HitResult.GetActor());

	if (!bHit || (IsValid(HitActor) && HitActor->IsOwnedBy(this)))
	{
		OutSeenLocation = bHit ? HitResult.ImpactPoint : OutSeenLocation;
		OutNumberOfLoSChecksPerformed = 1;
		OutNumberOfAsyncLosCheckRequested = 0;
		OutSightStrength = 1;
		//UE_LOG(LogTemp, Warning, TEXT("%s is visible"), *this->GetName());
		return UAISense_Sight::EVisibilityResult::Visible;
	}

	OutNumberOfLoSChecksPerformed = 1;
	OutNumberOfAsyncLosCheckRequested = 0;
	OutSightStrength = 0;
	//UE_LOG(LogTemp, Warning, TEXT("%s is invisible"), *this->GetName());
	return UAISense_Sight::EVisibilityResult::NotVisible;
}

void AUmbraPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->SetRenderCustomDepth(true);	
	if (IsLocallyControlled())
	{
		GetMesh()->SetCustomDepthStencilValue(77);
	}
	else
	{
		GetMesh()->SetCustomDepthStencilValue(XRAY_STENCIL_VALUE);
	}

	// FGameplayTagContainer TC;
	// TC.AddTag(FUmbraGameplayTags::Get().Ability_Interact);
	// AbilitySystemComponent->TryActivateAbilitiesByTag(TC);
}

void AUmbraPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

void AUmbraPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AUmbraPlayerCharacter::InitAbilityActorInfo()
{
	AUmbraPlayerState* UmbraPlayerState = GetPlayerState<AUmbraPlayerState>();
	checkf(UmbraPlayerState, TEXT("Player State is invalid in [%s]"), *GetNameSafe(this));
	UmbraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(UmbraPlayerState, this);
	AbilitySystemComponent = Cast<UUmbraAbilitySystemComponent>(UmbraPlayerState->GetAbilitySystemComponent());

	VitalAttributeSet = UmbraPlayerState->GetVitalAttributeSet();
	MovementAttributeSet = UmbraPlayerState->GetMovementAttributeSet();
	StealthAttributeSet = UmbraPlayerState->GetStealthAttributeSet();
	
	ApplyStartingEffects();
}
