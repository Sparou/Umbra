// Copyrighted by Vorona Games


#include "Actors/UmbraSoundDome.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


AUmbraSoundDome::AUmbraSoundDome()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	DomeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("DomeCollision"));
	RootComponent = DomeCollision;
	DomeCollision->OnComponentBeginOverlap.AddDynamic(this, &AUmbraSoundDome::OnOverlapBegin);
	DomeCollision->OnComponentEndOverlap.AddDynamic(this, &AUmbraSoundDome::OnOverlapEnd);
}

void AUmbraSoundDome::Init(float Radius, float LifeSpan)
{
	if (DomeCollision)
	{
		DomeCollision->SetSphereRadius(Radius);
	}
	SetLifeSpan(LifeSpan);  // Портал исчезнет через LifeSpan секунд
}

void AUmbraSoundDome::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                     bool bFromSweep, const FHitResult& SweepResult)
{
	if (SilenceMix)
	{
		UGameplayStatics::PushSoundMixModifier(this, SilenceMix);
	}
}

void AUmbraSoundDome::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (SilenceMix)
	{
		UGameplayStatics::PopSoundMixModifier(this, SilenceMix);
	}
}