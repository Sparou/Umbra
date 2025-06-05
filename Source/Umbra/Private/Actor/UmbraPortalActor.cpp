// Copyrighted by Vorona Games


#include "Actor/UmbraPortalActor.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

TArray<AActor*> AUmbraPortalActor::RecentlyTeleportedActors;

void AUmbraPortalActor::BeginPlay()
{
	Super::BeginPlay();
	RecentlyTeleportedActors = TArray<AActor*>();

	// Устанавливаем время жизни портала
	SetLifeSpan(LifeSpan);  // Портал исчезнет через LifeSpan секунд
}

void AUmbraPortalActor::Init(FVector TrgLocation)
{
	TargetLocation = TrgLocation;

	// Привязываем событие для начала перекрытия
	PortalCollision->OnComponentBeginOverlap.AddDynamic(this, &AUmbraPortalActor::OnOverlapBegin);
}

AUmbraPortalActor::AUmbraPortalActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Привязываем компонент Capsule, который в Blueprint называется "Portal"
	PortalCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PortalCollision"));
	RootComponent = PortalCollision;
    
	// Настроим параметры коллизии
	PortalCollision->SetCapsuleRadius(100.f);
	PortalCollision->SetCapsuleHalfHeight(200.f);
	PortalCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	PortalCollision->SetGenerateOverlapEvents(true);
	PortalCollision->SetIsReplicated(true);
	PortalCollision->SetHiddenInGame(false);
}

void AUmbraPortalActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (!RecentlyTeleportedActors.Contains(OtherActor))
		{
			RecentlyTeleportedActors.Add(OtherActor);
			UE_LOG(LogTemp, Warning, TEXT("Teleporting %s"), *RecentlyTeleportedActors[0]->GetName());

			// Используй СТАТИЧЕСКИЙ метод или лямбду:
			FTimerDelegate TimerDel;
			TimerDel.BindLambda([=]()
			{
				RecentlyTeleportedActors.Remove(OtherActor);
			});

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.f, false);
			
			Character->SetActorLocation(TargetLocation);
		}
	}
}