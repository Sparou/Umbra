// Copyrighted by Vorona Games


#include "Actors/UmbraArrowProjectile.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AUmbraArrowProjectile::AUmbraArrowProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	Mesh->SetSimulatePhysics(false);
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->SetCollisionProfileName("BlockAll");
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); 
	
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->UpdatedComponent = Mesh;
	
	AutoPossessAI = EAutoPossessAI::Disabled;
}

void AUmbraArrowProjectile::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnComponentHit.AddDynamic(this, &AUmbraArrowProjectile::OnHit);
	UE_LOG(LogTemp, Error, TEXT("AUmbraArrowProjectile::BeginPlay"));
}

void AUmbraArrowProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
							 UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		if (UAbilitySystemComponent* ASC = OtherActor->FindComponentByClass<UAbilitySystemComponent>())
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(HitEffect, 1.0f, EffectContext);
		
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	Destroy();
}