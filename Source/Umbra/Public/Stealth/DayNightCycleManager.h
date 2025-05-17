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
	ULightComponent* Light;

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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float DayLengthInSeconds = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	ADirectionalLight* SunLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	ASkyLight* SkyLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	AActor* SkySphereActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	ADirectionalLight* MoonLight;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Lights")
	float NightThreshold = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Lights")
	float FadeSpeed = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Lights")
	TArray<AActor*> NightActiveActors;
	

private:
	float CurrentTime = 0.f;
	bool bIsNight = false;

	UPROPERTY()
	TArray<FManagedLight> ToggleableLights;

	UPROPERTY()
	TArray<UCandleFlickerComponent*> CandleFlickers;

	UPROPERTY()
	TArray<UMaterialSwitcherComponent*> MaterialSwitchers;

	void UpdateLighting(float NormalizedTime);
	void CollectAllNightLights();
	void UpdateNightLights(float SunFactor, float DeltaTime);
	void UpdateNightActors(bool bEnable);
	void UpdateMaterialSwitchers(bool bNight);
};
