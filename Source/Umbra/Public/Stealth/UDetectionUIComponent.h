#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UDetectionUIComponent.generated.h"

class UUserWidget;
class UProgressBar;
class UTextBlock;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API UDetectionUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDetectionUIComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> DetectionWidgetClass;

	UPROPERTY()
	UUserWidget* DetectionWidget;

	UPROPERTY()
	UProgressBar* ProgressBar;

	UPROPERTY()
	UTextBlock* DetectionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float DetectionValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float DetectionSpeed = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool bIsPlayerVisible = false;

private:
	void UpdateUI(float DeltaTime);
};
