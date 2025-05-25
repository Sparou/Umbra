#include "Stealth/NPCDetection.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Stealth/LightingDetection.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Stealth/UDetectionUIComponent.h"

// Sets default values
ANPCDetection::ANPCDetection()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANPCDetection::BeginPlay()
{
	Super::BeginPlay();

	// Получаем ссылку на игрока
	PlayerCharacter = Cast<AUmbraPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (!PlayerCharacter)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Не удалось найти AUmbraPlayerCharacter"));
	}
}

// Called every frame
void ANPCDetection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerCharacter)
	{
		return;
	}

	// Проверка видимости
	bool bPlayerVisible = CanSeePlayer();

	if (bPlayerVisible)
	{
		GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Green, TEXT("NPC видит вас!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Red, TEXT("NPC вас не видит"));
	}

	// Передаём информацию в UI
	if (UDetectionUIComponent* UIComponent = PlayerCharacter->FindComponentByClass<UDetectionUIComponent>())
	{
		UIComponent->bIsPlayerVisible = bPlayerVisible;
	}
}


bool ANPCDetection::CanSeePlayer()
{
	// Проверка расстояния
	float Distance = FVector::Distance(PlayerCharacter->GetActorLocation(), GetActorLocation());
	if (Distance > VisionRadius)
	{
		return false; // Игрок слишком далеко
	}

	// Проверка уровня освещенности
	ULightingDetection* LightingDetector = PlayerCharacter->FindComponentByClass<ULightingDetection>();
	if (!LightingDetector || LightingDetector->LightPercentage < MinLightLevel)
	{
		return false; // Освещенность недостаточная
	}

	// Проверка линии видимости с учетом коллизий
	if (!HasLineOfSight())
	{
		return false; // Есть преграда на пути
	}

	return true;
}

// Функция для проверки линии видимости
bool ANPCDetection::HasLineOfSight()
{
	// Трассировка от NPC до игрока
	FHitResult HitResult;
	FVector Start = GetActorLocation() + FVector(0, 0, 50); // Трассировка с небольшим смещением
	FVector End = PlayerCharacter->GetActorLocation() + FVector(0, 0, 50); // Трассировка до уровня головы игрока

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // Игнорируем сам NPC
	Params.AddIgnoredActor(PlayerCharacter); // Игнорируем самого игрока

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	// Если трассировка зацепила что-то, то на пути есть преграда
	return !bHit;
}
