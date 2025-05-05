#include "Stealth/CandleFlickerComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Actor.h"

UCandleFlickerComponent::UCandleFlickerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCandleFlickerComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<ULightComponent*> LightComponents;
	GetOwner()->GetComponents<ULightComponent>(LightComponents);
	for (ULightComponent* Comp : LightComponents)
	{
		if (auto* AsPoint = Cast<UPointLightComponent>(Comp))
		{
			PointLight = AsPoint;
			Light = AsPoint;
			BaseAttenuationRadius = PointLight->AttenuationRadius;
			break;
		}
		else if (auto* AsSpot = Cast<USpotLightComponent>(Comp))
		{
			SpotLight = AsSpot;
			Light = AsSpot;
			BaseAttenuationRadius = SpotLight->AttenuationRadius;
			break;
		}
	}

	if (Light)
	{
		BaseIntensity = Light->Intensity;
		CurrentFlickerSpeed = BaseFlickerSpeed;
	}
}

void UCandleFlickerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Light) return;

	FlickerSpeedTimer += DeltaTime;
	if (FlickerSpeedTimer > 1.0f)
	{
		UpdateFlickerSpeed();
		FlickerSpeedTimer = 0.f;
	}

	TimeAccumulator += DeltaTime * CurrentFlickerSpeed;

	float Noise = FMath::Sin(TimeAccumulator * 1.5f) + FMath::FRandRange(-0.5f, 0.5f);
	float NewIntensity = BaseIntensity + Noise * IntensityFlickerAmount;
	NewIntensity = FMath::Clamp(NewIntensity, 0.f, BaseIntensity + IntensityFlickerAmount);
	Light->SetIntensity(NewIntensity);

	// Обновление радиуса
	float RadiusNoise = FMath::FRandRange(-1.f, 1.f);
	float NewRadius = BaseAttenuationRadius + RadiusNoise * RadiusFlickerAmount;
	NewRadius = FMath::Clamp(NewRadius, 100.f, BaseAttenuationRadius + RadiusFlickerAmount);

	if (PointLight)
	{
		PointLight->SetAttenuationRadius(NewRadius);
	}
	else if (SpotLight)
	{
		SpotLight->SetAttenuationRadius(NewRadius);
	}

	// Обновление цвета
	FLinearColor ColorOffset = FLinearColor(
		FMath::FRandRange(-ColorFlickerRange.R, ColorFlickerRange.R),
		FMath::FRandRange(-ColorFlickerRange.G, ColorFlickerRange.G),
		FMath::FRandRange(-ColorFlickerRange.B, ColorFlickerRange.B));
	FLinearColor NewColor = (BaseLightColor + ColorOffset).GetClamped();
	Light->SetLightColor(NewColor);
}

void UCandleFlickerComponent::UpdateFlickerSpeed()
{
	CurrentFlickerSpeed = BaseFlickerSpeed + FMath::FRandRange(-FlickerSpeedVariance, FlickerSpeedVariance);
	CurrentFlickerSpeed = FMath::Max(1.f, CurrentFlickerSpeed);
}
