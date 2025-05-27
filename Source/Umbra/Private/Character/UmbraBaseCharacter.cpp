// Copyrighted by Vorona Games

#include "Character/UmbraBaseCharacter.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"
#include "Character/Component/TagManager.h"
#include "TraversalSystem/TraversalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Umbra/Umbra.h"

AUmbraBaseCharacter::AUmbraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("Motion Warping");
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Weapon Mesh");
	WeaponMeshComponent->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	AbilitySystemComponent = CreateDefaultSubobject<UUmbraAbilitySystemComponent>("Ability System");
	TagManager = CreateDefaultSubobject<UTagManager>("Tag Manager");

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

void AUmbraBaseCharacter::EnableOutline_Implementation(int32 StencilValue)
{
	if (GetMesh()->bRenderCustomDepth == true && GetMesh()->CustomDepthStencilValue == XRAY_STENCIL_VALUE)
	{
		return;
	}
	
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(StencilValue);
}

void AUmbraBaseCharacter::DisableOutline_Implementation()
{
	if (GetMesh()->CustomDepthStencilValue != XRAY_STENCIL_VALUE)
	{
		GetMesh()->SetRenderCustomDepth(false);
	}
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

void AUmbraBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AUmbraBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
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

void AUmbraBaseCharacter::Dissolve()
{
	if (IsValid(DissolveMaterial))
	{
		UMaterialInstanceDynamic* DM = UMaterialInstanceDynamic::Create(DissolveMaterial, this);
		GetMesh()->SetMaterial(0, DM);
		GetMesh()->SetMaterial(1, DM);
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
	
	bIsDead = true;
	OnDeathDelegate.Broadcast();
}
