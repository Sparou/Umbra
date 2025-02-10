// Copyrighted by Vorona Games

#include "Character/UmbraBaseCharacter.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "TraversalSystem/TraversalComponent.h"
#include "Components/CapsuleComponent.h"

AUmbraBaseCharacter::AUmbraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("Motion Warping");
	TraversalComponent = CreateDefaultSubobject<UTraversalComponent>("Traversal Component");
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	WeaponMeshComponent->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	AbilitySystemComponent = CreateDefaultSubobject<UUmbraAbilitySystemComponent>("Ability System");
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

UAnimMontage* AUmbraBaseCharacter::GetRandomHitReactMontage_Implementation()
{
	return HitReactMontages.Num() > 0 ? HitReactMontages[FMath::RandRange(0, HitReactMontages.Num() - 1)] : nullptr;
}

UNiagaraSystem* AUmbraBaseCharacter::GetBloodEffect_Implementation() const
{
	return nullptr;
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

void AUmbraBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		InitAbilityActorInfo();
		InitializeDefaultAttributes();
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
	//UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	
	WeaponMeshComponent->SetSimulatePhysics(true);
	WeaponMeshComponent->SetEnableGravity(true);
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bIsDead = true;
}
