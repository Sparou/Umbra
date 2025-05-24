#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CandleFlickerComponent.generated.h"

class ULightComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UMBRA_API UCandleFlickerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCandleFlickerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Flicker")
	float BaseIntensity = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Flicker")
	float IntensityFlickerAmount = 600.f;

	UPROPERTY(EditAnywhere, Category = "Flicker")
	float RadiusFlickerAmount = 100.f;

	UPROPERTY(EditAnywhere, Category = "Flicker")
	float BaseAttenuationRadius = 600.f;

	UPROPERTY(EditAnywhere, Category = "Flicker")
	FLinearColor BaseLightColor = FLinearColor(1.0f, 0.55f, 0.3f);

	UPROPERTY(EditAnywhere, Category = "Flicker")
	FLinearColor ColorFlickerRange = FLinearColor(0.05f, 0.03f, 0.02f);

	UPROPERTY(EditAnywhere, Category = "Flicker")
	float BaseFlickerSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "Flicker")
	float FlickerSpeedVariance = 5.f;

private:
	ULightComponent* Light = nullptr;
	class UPointLightComponent* PointLight = nullptr;
	class USpotLightComponent* SpotLight = nullptr;

	float TimeAccumulator = 0.f;
	float CurrentFlickerSpeed = 10.f;
	float FlickerSpeedTimer = 0.f;

	void UpdateFlickerSpeed();
};
