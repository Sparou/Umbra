#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CandleFlickerComponent.generated.h"

class ULightComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API UCandleFlickerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCandleFlickerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Базовая сила света */
	UPROPERTY(EditAnywhere, Category = "Flicker")
	float BaseIntensity = 5000.f;

	/** Максимальная амплитуда колебаний */
	UPROPERTY(EditAnywhere, Category = "Flicker")
	float FlickerAmount = 500.f;

	/** Скорость мерцания */
	UPROPERTY(EditAnywhere, Category = "Flicker")
	float FlickerSpeed = 10.f;

private:
	ULightComponent* Light = nullptr;
	float TimeAccumulator = 0.f;
};
