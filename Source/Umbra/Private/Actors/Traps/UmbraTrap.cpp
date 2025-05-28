// Copyrighted by Vorona Games


#include "Actors/Traps/UmbraTrap.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/UmbraAbilitySystemComponent.h"


// Sets default values
AUmbraTrap::AUmbraTrap()
{
	bReplicates = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Привязываем компонент Capsule, который в Blueprint называется "Portal"
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PortalCollision"));
	RootComponent = SphereCollision;
    
	// Настроим параметры коллизии
	SphereCollision->SetSphereRadius(100.f);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetIsReplicated(true);
	SphereCollision->SetHiddenInGame(false);
}

// Called when the game starts or when spawned
void AUmbraTrap::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AUmbraTrap::OnOverlapBegin);
	SetLifeSpan(LifeSpan);
}


void AUmbraTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (!OtherActor || !InitializedCharacter || Cast<AUmbraPlayerCharacter>(OtherActor) == InitializedCharacter)
	{
		// Игнорируем своего владельца 
		return;
	}

	UAbilitySystemComponent* ASC = Cast<UUmbraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor));
	if (ASC && Effect)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1.0f, EffectContext);
		
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			// Получаем все активные теги
			FGameplayTagContainer ActiveTags;
			ASC->GetOwnedGameplayTags(ActiveTags);

			for (const FGameplayTag& Tag : ActiveTags)
			{
				UE_LOG(LogTemp, Log, TEXT("Active Tag: %s"), *Tag.ToString());
			}
		}
	}
	Destroy();
}
