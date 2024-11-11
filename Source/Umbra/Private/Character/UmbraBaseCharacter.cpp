// Copyrighted by Vorona Games

#include "Character/UmbraBaseCharacter.h"

#include "CharacterTrajectoryComponent.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "AbilitySystem/UmbraAttributeSet.h"
#include "Components/CapsuleComponent.h"

AUmbraBaseCharacter::AUmbraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("Motion Warping");
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	WeaponMesh->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	AbilitySystemComponent = CreateDefaultSubobject<UUmbraAbilitySystemComponent>("Ability System");
	AttributeSet = CreateDefaultSubobject<UUmbraAttributeSet>("Attribute Set");
	TrajectoryComponent = CreateDefaultSubobject<UCharacterTrajectoryComponent>("Character Trajectory");
}

void AUmbraBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
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
