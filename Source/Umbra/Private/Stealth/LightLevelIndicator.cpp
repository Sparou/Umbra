// Copyrighted by Vorona Games


#include "Stealth/LightLevelIndicator.h"
#include "Components/ProgressBar.h"
#include "Stealth/LightingDetection.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void ULightLevelIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	// Найдём у игрока компонент освещённости
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (APawn* P = PC->GetPawn())
		{
			LightingDetector = P->FindComponentByClass<ULightingDetection>();
			if (!LightingDetector)
				UE_LOG(LogTemp, Error, TEXT("LightLevelIndicator: no LightingDetection on pawn"));
		}
	}
}

void ULightLevelIndicator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (LightingDetector && ProgressBar_LightLevel)
	{
		float P = LightingDetector->GetLightPercentage() / 100.0f;
		ProgressBar_LightLevel->SetPercent(P);
	}
}
