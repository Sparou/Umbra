// Copyrighted by Vorona Games

#include "Character/UmbraBaseCharacter.h"

#include <string>

#include "CharacterTrajectoryComponent.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"

AUmbraBaseCharacter::AUmbraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("Motion Warping");
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	WeaponMesh->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	AbilitySystemComponent = CreateDefaultSubobject<UUmbraAbilitySystemComponent>("Ability System");
	TrajectoryComponent = CreateDefaultSubobject<UCharacterTrajectoryComponent>("Character Trajectory");
}

void AUmbraBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
	InitializeDefaultAttributes();
}

void AUmbraBaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const
{
	check(IsValid(AbilitySystemComponent));
	check(GameplayEffectClass);
	
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
}

void AUmbraBaseCharacter::InitializeDefaultAttributes() const
{
	//TODO: Make Level variable
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AUmbraBaseCharacter::InitAbilityActorInfo()
{
	if (UUmbraAbilitySystemComponent* UmbraASC = Cast<UUmbraAbilitySystemComponent>(AbilitySystemComponent))
	{
		UmbraASC->AddCharacterAbilities(StartingAbilities);
	}
}

void AUmbraBaseCharacter::MulticastHandleDeath_Implementation()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
  
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("Motion Warping");
  
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	WeaponMesh->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
