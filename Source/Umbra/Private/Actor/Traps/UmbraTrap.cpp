// Copyrighted by Vorona Games


#include "Actor/Traps/UmbraTrap.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
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
	
    Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Plane->SetupAttachment(RootComponent);
	
	// Настроим параметры коллизии
	SphereCollision->SetSphereRadius(100.f);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetIsReplicated(true);
	SphereCollision->SetHiddenInGame(false);
}

void AUmbraTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (UMaterialInstanceDynamic* MaterialInstance = Plane->CreateAndSetMaterialInstanceDynamic(0))
	{
		static float DissolveValue = -0.5f;
		static float DissolveSpeed = 0.01f;
		static bool bIncreasing = true;

		// Update DissolveValue
		if (bIncreasing)
		{
			DissolveValue += DissolveSpeed;
			if (DissolveValue >= 0.5f)
			{
				DissolveValue = 0.5f;
				bIncreasing = false;
			}
		}
		else
		{
			DissolveValue -= DissolveSpeed;
			if (DissolveValue <= -0.5f)
			{
				DissolveValue = -0.5f;
				bIncreasing = true;
			}
		}
		
		MaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

// Called when the game starts or when spawned
void AUmbraTrap::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle ActivateCollision;
	GetWorld()->GetTimerManager().SetTimer(ActivateCollision, this, &AUmbraTrap::ActivateTrap, 1.0f, false);
	SetLifeSpan(LifeSpan);
}

void AUmbraTrap::ActivateTrap()
{
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AUmbraTrap::OnOverlapBegin);
}


void AUmbraTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
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
		}
	}
}
