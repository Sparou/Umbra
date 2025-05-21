#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayNightCycleManager.generated.h"

class ADirectionalLight;
class ASkyLight;
class ULightComponent;
class UCandleFlickerComponent;
class UMaterialSwitcherComponent;

USTRUCT()
struct FManagedLight
{
	GENERATED_BODY()

	UPROPERTY()
	ULightComponent* Light;

	UPROPERTY()
	float BaseIntensity;

	FManagedLight() : Light(nullptr), BaseIntensity(1.f) {}
	FManagedLight(ULightComponent* InLight)
		: Light(InLight), BaseIntensity(InLight ? InLight->Intensity : 1.f) {}
};

UCLASS()
class STEALTH_API ADayNightCycleManager : public AActor
{
	GENERATED_BODY()
	
public:
	ADayNightCycleManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Длительность суток в секундах (например, 120 = 2 минуты)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float DayLengthInSeconds = 240.f;

	// Время начала суток при старте игры (в секундах)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float StartTimeOfDay = 0.f;

	// Порог для активации ночного режима (0..1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float NightThreshold = 0.2f;

	// Максимальная интенсивность света солнца
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Sun")
	float SunMaxIntensity = 10.f;

	// Максимальная интенсивность света луны
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Moon")
	float MoonMaxIntensity = 1.f;

	// Интенсивность SkyLight ночью
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Ambient")
	float IndoorAmbientIntensity = 0.2f;

	// Цвет SkyLight ночью
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Ambient")
	FLinearColor IndoorAmbientColor = FLinearColor(0.015f, 0.02f, 0.04f);

	// Цвет неба днём
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Colors")
	FLinearColor DayColor = FLinearColor(1.f, 0.95f, 0.85f);

	// Цвет неба ночью
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Colors")
	FLinearColor NightColor = FLinearColor(0.015f, 0.02f, 0.04f);

	// Скорость перехода интенсивности света (для ламп и свечей)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight|Fade")
	float FadeSpeed = 2.f;

	// Ссылка на Directional Light, используемый как солнце
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ADirectionalLight* SunLight;

	// Ссылка на Directional Light, используемый как луна
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ADirectionalLight* MoonLight;

	// Ссылка на SkyLight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ASkyLight* SkyLight;

	// Актор SkySphere (если используется)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	AActor* SkySphereActor;

	// Акторы, активирующиеся только ночью
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Night")
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

	void CollectAllNightLights();
	void UpdateLighting(float NormalizedTime);
	void UpdateNightLights(float SunFactor, float DeltaTime);
	void UpdateNightActors(bool bEnable);
	void UpdateMaterialSwitchers(bool bNight);
};
