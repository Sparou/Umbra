#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/LightComponent.h"      
#include "DayNightCycleManager.generated.h"

class ADirectionalLight;
class ASkyLight;
class UCandleFlickerComponent;
class UMaterialSwitcherComponent;

USTRUCT()
struct FManagedLight
{
	GENERATED_BODY()

	UPROPERTY()
	ULightComponent* Light = nullptr;

	float BaseIntensity = 0.f;

	FManagedLight() {}
	FManagedLight(ULightComponent* InLight)
		: Light(InLight), BaseIntensity(InLight->Intensity) {}
};

UCLASS()
class UMBRA_API ADayNightCycleManager : public AActor
{
	GENERATED_BODY()

public:
	ADayNightCycleManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Core update methods
	void UpdateLighting(float NormalizedTime);
	void UpdateNightLights(float SunFactor, float DeltaTime);
	void UpdateNightActors(bool bEnable);
	void UpdateMaterialSwitchers(bool bNight);
	void CollectAllNightLights();

public:
	// Day length in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Time", meta = (ClampMin = "1.0"))
	float DayLengthInSeconds = 600.f;

	// Start time offset in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Time", meta = (ClampMin = "0.0"))
	float StartTimeOfDay = 0.f;

	// Sun and Moon directional lights
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Lights")
	ADirectionalLight* SunLight = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Lights")
	ADirectionalLight* MoonLight = nullptr;

	// Global skylight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Lights")
	ASkyLight* SkyLight = nullptr;

	// Optional BP_Sky_Sphere actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Sky")
	AActor* SkySphereActor = nullptr;

	// Threshold to consider "night"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Lights")
	float NightThreshold = 0.15f;

	// Speed for fading local lights
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Lights")
	float FadeSpeed = 1.5f;

	// Actors to enable at night (e.g. lamps)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Lights")
	TArray<AActor*> NightActiveActors;

	// Sky colors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Colors")
	FLinearColor NightColor = FLinearColor(0.02f, 0.05f, 0.2f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Colors")
	FLinearColor DawnColor = FLinearColor(1.0f, 0.55f, 0.25f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Colors")
	FLinearColor DayColor = FLinearColor(1.0f, 1.0f, 0.9f);

	// Intensities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Intensity")
	float SunMaxIntensity = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Intensity")
	float MoonMaxIntensity = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Intensity")
	float SkyLightMaxIntensity = 3.0f;

	// Indoor ambient when sun below horizon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Intensity")
	FLinearColor IndoorAmbientColor = FLinearColor(0.1f, 0.1f, 0.15f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Intensity")
	float IndoorAmbientIntensity = 0.5f;

private:
	float CurrentTime = 0.f;
	bool  bIsNight    = false;

	UPROPERTY()
	TArray<FManagedLight> ToggleableLights;

	UPROPERTY()
	TArray<UCandleFlickerComponent*> CandleFlickers;

	UPROPERTY()
	TArray<UMaterialSwitcherComponent*> MaterialSwitchers;
};
