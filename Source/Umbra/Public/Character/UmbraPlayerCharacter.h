#pragma once

#include "CoreMinimal.h"
#include "UmbraCoreTypes.h"
#include "Character/UmbraBaseCharacter.h"
#include "Stealth/LightingDetection.h"
#include "Blueprint/UserWidget.h"
#include "UmbraPlayerCharacter.generated.h"

class UInteractionComponent;

UCLASS()
class UMBRA_API AUmbraPlayerCharacter : public AUmbraBaseCharacter
{
	GENERATED_BODY()

public:
	// Конструктор по-умолчанию
	AUmbraPlayerCharacter();

	UFUNCTION(BlueprintCallable)
	AActor* GetOverlappedActorFromInteractionComponent() const;

	UFUNCTION(BlueprintCallable)
	FStealthKillMontages GetRandomStealthKillMontages(TArray<FStealthKillMontages> KillMontages) const;

	UFUNCTION(BlueprintCallable)
	FStealthKillMontages GetStealthKillMontageForPosition(EStealthKillPosition KillPosition);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void InitAbilityActorInfo() override;

	/** Компонент взаимодействия */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	/** Анимации для добивок */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|Animations")
	TArray<FStealthKillMontages> StealthKillMontagesFromBehind;
	// ... остальные массивы аналогично

private:
	/** Компонент определения освещённости */
	UPROPERTY(VisibleAnywhere, Category = "Lighting")
	ULightingDetection* LightingDetector;

	/** Наш виджет-класс полоски */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LightWidgetClass;
};
