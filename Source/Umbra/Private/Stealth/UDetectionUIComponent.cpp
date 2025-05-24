#include "Stealth/UDetectionUIComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

UDetectionUIComponent::UDetectionUIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDetectionUIComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DetectionWidgetClass)
	{
		DetectionWidget = CreateWidget<UUserWidget>(GetWorld(), DetectionWidgetClass);

		if (DetectionWidget)
		{
			DetectionWidget->AddToViewport();

			// Получаем доступ к виджетам по имени
			ProgressBar = Cast<UProgressBar>(DetectionWidget->GetWidgetFromName(TEXT("DetectionProgressBar")));
			DetectionText = Cast<UTextBlock>(DetectionWidget->GetWidgetFromName(TEXT("DetectedText")));

			if (DetectionText)
			{
				DetectionText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UDetectionUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateUI(DeltaTime);
}

void UDetectionUIComponent::UpdateUI(float DeltaTime)
{
	if (!ProgressBar || !DetectionText)
		return;

	// Увеличиваем или уменьшаем значение в зависимости от видимости игрока
	if (bIsPlayerVisible)
	{
		DetectionValue = FMath::Clamp(DetectionValue + DeltaTime * DetectionSpeed, 0.f, 1.f);
	}
	else
	{
		DetectionValue = FMath::Clamp(DetectionValue - DeltaTime * DetectionSpeed, 0.f, 1.f);
	}

	// Обновляем прогрессбар
	ProgressBar->SetPercent(DetectionValue);

	// Показываем текст "Вас заметили", если заполнено полностью
	if (DetectionValue >= 1.f)
	{
		DetectionText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		DetectionText->SetVisibility(ESlateVisibility::Hidden);
	}
}
