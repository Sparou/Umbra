// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LightLevelIndicator.generated.h"

class UProgressBar;
class ULightingDetection;

UCLASS()
class UMBRA_API ULightLevelIndicator : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Вызывается после того, как виджет построен
	virtual void NativeConstruct() override;

	// Вызывается каждую итерацию UI
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Автоматически свяжется с элементом UMG с именем ProgressBar_LightLevel
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_LightLevel;

private:
	// Сюда запишем компонент освещённости игрока
	UPROPERTY()
	ULightingDetection* LightingDetector = nullptr;
};