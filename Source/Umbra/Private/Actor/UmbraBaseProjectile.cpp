// Copyrighted by Vorona Games

#include "Actor/UmbraBaseProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

DEFINE_LOG_CATEGORY(UmbraProjectileLog)

AUmbraBaseProjectile::AUmbraBaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere Component");
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement Component");

	SetRootComponent(SphereComponent);
}

void AUmbraBaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(UmbraProjectileLog, Log, TEXT("%s was created!"), *GetNameSafe(this));
	SetLifeSpan(LifeSpan);

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AUmbraBaseProjectile::OnSphereBeginOverlap);
}

void AUmbraBaseProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!GameplayEffectSpecHandle.IsValid() ||
		GameplayEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor)
	{
		return;
	}
	
	UE_LOG(UmbraProjectileLog, Log, TEXT("Projectile = [%s] | Other Actor = [%s]"),
		*GetNameSafe(this),
		*GetNameSafe(OtherActor));
	
	if(HasAuthority())
	{
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpecHandle.Data.Get());
		}
	}

	Destroy();
}