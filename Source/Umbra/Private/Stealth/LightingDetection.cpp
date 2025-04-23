// Copyrighted by Vorona Games


#include "Stealth/LightingDetection.h"

#include "Engine/DirectionalLight.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "Components/LightComponent.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Engine/DirectionalLight.h"

// Sets default values for this component's properties
ULightingDetection::ULightingDetection()
{
	PrimaryComponentTick.bCanEverTick = true;

	LightPercentage = 0.f;
	DetectionRadius = 900.f;      // Можно настроить под размеры уровня
	MaxLightValue = 10.f;       // Настраиваемый параметр для нормировки суммарного света
	DirectionalLightCoefficient =20.f; // Значение по умолчанию, можно менять в редакторе или через код
}


// Called when the game starts
void ULightingDetection::BeginPlay()
{
	Super::BeginPlay();
	
	
}


// Called every frame
void ULightingDetection::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Получаем владельца (игрока)
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	FVector OwnerLocation = Owner->GetActorLocation();

	float TotalLight = 0.f;

	// Проходим по всем акторам в мире
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor == Owner)
			continue;

		// Получаем компоненты заданного класса с помощью метода GetComponents
		TArray<UActorComponent*> LightComponents;
		Actor->GetComponents(ULightComponent::StaticClass(), LightComponents);

		if (LightComponents.Num() > 0)
		{
			for (UActorComponent* Comp : LightComponents)
			{
				ULightComponent* LightComp = Cast<ULightComponent>(Comp);
				if (LightComp)
				{
					// Для локальных источников учитываем расстояние
					float Distance = FVector::Dist(OwnerLocation, Actor->GetActorLocation());
					if (Distance <= DetectionRadius)
					{
						// Вклад: интенсивность делённая на max(расстояние, 1)
						float Contribution = LightComp->Intensity / FMath::Max(Distance, 1.f);
						TotalLight += Contribution;
					}
					// Для направленного света (например, солнца) учитываем коэффициент
					else if (Actor->IsA(ADirectionalLight::StaticClass()))
					{
						TotalLight += LightComp->Intensity * DirectionalLightCoefficient;
					}
				}
			}
		}
	}

	// Нормируем значение в диапазоне 0–100%
	float Percentage = FMath::Clamp(TotalLight / MaxLightValue, 0.f, 1.f) * 100.f;
	LightPercentage = Percentage;

	// Выводим уровень освещённости на экран для отладки
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("Light Level: %.1f%%"), LightPercentage);
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, DebugMsg);
	}
}

