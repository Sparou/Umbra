#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayNightCycleManager.generated.h"

class ADirectionalLight;
class ASkyLight;
class AActor;

UCLASS()
class UMBRA_API ADayNightCycleManager : public AActor
{
	GENERATED_BODY()

public:
	ADayNightCycleManager();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** Длительность полного дня в секундах */
	UPROPERTY(EditAnywhere, Category = "DayNight")
	float DayLengthInSeconds;

	/** Направленный свет (солнце) */
	UPROPERTY(EditAnywhere, Category = "DayNight")
	ADirectionalLight* SunLight;

	/** Небесный свет (окружающий свет) */
	UPROPERTY(EditAnywhere, Category = "DayNight")
	ASkyLight* SkyLight;

	/** Актор небесной сферы (BP_Sky_Sphere) */
	UPROPERTY(EditAnywhere, Category = "DayNight")
	AActor* SkySphereActor;

private:
	float CurrentTime;
	void UpdateLighting(float NormalizedTime);
};
