// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LightingDetection.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API ULightingDetection : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULightingDetection();
	
	/** Текущий уровень освещённости в процентах (0-100%) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
	float LightPercentage;

	/** Радиус, в пределах которого ищем источники света */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
	float DetectionRadius;

	/** Максимальное значение суммарного света для нормировки */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
	float MaxLightValue;

	/** Коэффициент для направленного источника света (например, солнца) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
	float DirectionalLightCoefficient;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
