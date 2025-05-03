// Copyrighted by Vorona Games


#include "Stealth/CandleFlickerComponent.h"
#include "Components/LightComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"

UCandleFlickerComponent::UCandleFlickerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCandleFlickerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Пытаемся найти первый компонент света
	TArray<ULightComponent*> LightComponents;
	GetOwner()->GetComponents<ULightComponent>(LightComponents);
	if (LightComponents.Num() > 0)
	{
		Light = LightComponents[0];
		if (Light)
		{
			BaseIntensity = Light->Intensity;
		}
	}
}

void UCandleFlickerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Light)
	{
		TimeAccumulator += DeltaTime * FlickerSpeed;

		// Мерцание — синус + небольшой рандом
		float Flicker = FMath::Sin(TimeAccumulator) + FMath::FRandRange(-0.2f, 0.2f);
		float NewIntensity = BaseIntensity + Flicker * FlickerAmount;
		NewIntensity = FMath::Clamp(NewIntensity, 0.f, BaseIntensity + FlickerAmount);

		Light->SetIntensity(NewIntensity);
	}
}
