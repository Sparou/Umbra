// Copyrighted by Vorona Games

#include "Character/UmbraBaseCharacter.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Projectile/UmbraProjectileGameplayAbility.h"
#include "Character/Component/TagManager.h"
#include "Character/Component/TraversalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySpec.h"
#include "Net/UnrealNetwork.h"
#include "Umbra/Umbra.h"

AUmbraBaseCharacter::AUmbraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("Motion Warping");
	AbilitySystemComponent = CreateDefaultSubobject<UUmbraAbilitySystemComponent>("Ability System");
	TagManager = CreateDefaultSubobject<UTagManager>("Tag Manager");
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Weapon Mesh");
	WeaponMeshComponent->SetupAttachment(GetMesh(), "RWeaponSocket");

	GetCharacterMovement()->MaxWalkSpeed = StandRunSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchRunSpeed;
}

float AUmbraBaseCharacter::GetMoveSpeed(const FGameplayTag& Stance, const FGameplayTag& Locomotion)
{
	FUmbraGameplayTags UGT = FUmbraGameplayTags::Get();
	if (Stance == UGT.State_Stance_Standing)
	{
		if (Locomotion == UGT.State_Locomotion_Walking) return StandWalkSpeed;
		if (Locomotion == UGT.State_Locomotion_Running) return StandRunSpeed;
		return StandRunSpeed;
	}
	if (Stance == UGT.State_Stance_Crouching)
	{
		if (Locomotion == UGT.State_Locomotion_Walking) return CrouchWalkSpeed;
		if (Locomotion == UGT.State_Locomotion_Running) return CrouchRunSpeed;
		return CrouchRunSpeed;
	}
	return StandRunSpeed;
}

FWeaponSocketLocations AUmbraBaseCharacter::GetWeaponSocketLocations_Implementation() const
{
	if (WeaponMeshComponent && WeaponMeshComponent->DoesSocketExist(WeaponBaseSocketName) && WeaponMeshComponent->DoesSocketExist(WeaponTipSocketName))
	{
		FWeaponSocketLocations SocketLocations;
		SocketLocations.WeaponBase = WeaponMeshComponent->GetSocketLocation(WeaponBaseSocketName);
		SocketLocations.WeaponTip = WeaponMeshComponent->GetSocketLocation(WeaponTipSocketName);
		return SocketLocations;
	} 
	UE_LOG(LogTemp, Error, TEXT("Weapon sockets are not set up properly!"));
	return FWeaponSocketLocations();
}

FVector AUmbraBaseCharacter::GetProjectileSpawnLocation_Implementation() const
{
	return GetMesh()->GetSocketLocation("ProjectileSpawnSocket");
}

void AUmbraBaseCharacter::SetWarp_Implementation(FName WarpName, FVector TargetLocation, FRotator TargetRotation)
{
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(WarpName, TargetLocation, TargetRotation);
}

UAnimMontage* AUmbraBaseCharacter::GetRandomHitReactMontage_Implementation(FGameplayAbilityActivationInfo AbilityActivationInfo, float SeedMultiplier)
{
	FRandomStream RandomStream(AbilityActivationInfo.GetActivationPredictionKey().Current * SeedMultiplier);
	return HitReactMontages.Num() > 0 ? HitReactMontages[RandomStream.RandRange(0, HitReactMontages.Num() - 1)] : nullptr;
}

UAnimMontage* AUmbraBaseCharacter::GetRandomMeleeAttackMontage_Implementation(FGameplayAbilityActivationInfo AbilityActivationInfo,	float SeedMultiplier)
{
	FRandomStream RandomStream(AbilityActivationInfo.GetActivationPredictionKey().Current * SeedMultiplier);
	UE_LOG(LogTemp, Log, TEXT("Random Seed: %d"), RandomStream.GetCurrentSeed());
	return MeleeAttackMontages.Num() > 0 ? MeleeAttackMontages[RandomStream.RandRange(0, MeleeAttackMontages.Num() - 1)] : nullptr;
}

bool AUmbraBaseCharacter::IsDead_Implementation() const
{
	return bIsDead;
}

void AUmbraBaseCharacter::Die()
{
	WeaponMeshComponent->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

void AUmbraBaseCharacter::EnableOutline_Implementation(int32 StencilValue)
{
	if (GetMesh()->bRenderCustomDepth == true && GetMesh()->CustomDepthStencilValue == XRAY_STENCIL_VALUE)
	{
		return;
	}
	
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(ENEMY_OUTLINE_STENCIL_VALUE);
}

void AUmbraBaseCharacter::DisableOutline_Implementation()
{
	if (GetMesh()->CustomDepthStencilValue != XRAY_STENCIL_VALUE)
	{
		GetMesh()->SetRenderCustomDepth(false);
	}
}

UAbilitySystemComponent* AUmbraBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AUmbraBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AUmbraBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

UTagManager* AUmbraBaseCharacter::GetTagManager()
{
	return TagManager;
}

void AUmbraBaseCharacter::StartDissolve()
{
	if (HasAuthority())
	{
		MulticastDissolve();
	}
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
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
	ApplyEffectToSelf(DefaultStealthAttributes, 1.f);
}

void AUmbraBaseCharacter::InitAbilityActorInfo()
{
}

void AUmbraBaseCharacter::AddCharacterAbilities()
{
	if (!HasAuthority())
	{
		return;
	}
	AbilitySystemComponent->AddCharacterAbilities(StartingAbilities);
}

void AUmbraBaseCharacter::Dissolve()
{
	if (IsValid(DissolveMaterial))
	{
		UMaterialInstanceDynamic* DM = UMaterialInstanceDynamic::Create(DissolveMaterial, this);
		GetMesh()->SetMaterial(0, DM);
		StartDissolveTimeline(DM);
	}
}

void AUmbraBaseCharacter::MulticastDissolve_Implementation()
{
	Dissolve();
}

void AUmbraBaseCharacter::MulticastHandleDeath_Implementation()
{
	//UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	WeaponMeshComponent->SetSimulatePhysics(true);
	WeaponMeshComponent->SetEnableGravity(true);
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CharacterDeathDelegate.Broadcast();
	
	bIsDead = true;
}
