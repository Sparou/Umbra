// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StealthComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class FVisibilityCalculationTask;

DECLARE_LOG_CATEGORY_EXTERN(UmbraStealthComponentLog, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API UStealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStealthComponent();

protected:

	/** Имена сокетов на скелете владельца при проверке освещенности */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|LightDetection")
	TArray<FName> SampleSocketNames;

	/** Как часто (в секундах) проводить проверку уровня освещенности */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|LightDetection", meta = (ClampMin = "0.0"))
	float VisibilityUpdateInterval = 0.2f;

	/** Минимальный уровень заметности от отраженного света и в полной темноте */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|LightDetection", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinAmbientVisibility = 0.05f;

	/** Радиус поиска источников света, влияющих на заметность */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|LightDetection")
	float LightSearchRadius = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|LightDetection")
	TObjectPtr<UCurveFloat> FalloffCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|GAS")
	TSubclassOf<UGameplayEffect> VisibilityUpdateEffect;

protected:
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:

	void TriggerVisibilityCheck();
	
	FAsyncTask<FVisibilityCalculationTask>* CurrentVisibilityTask;

	FTimerHandle VisibilityCheckTimerHandle;
	
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> OwnerASC;

	UAbilitySystemComponent* GetOwnerASC();
};
