// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ObstacleDetectionSettings.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UObstacleDetectionSettings : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Obstacle Detection")
	float ObstacleDetectionOffset = 20.f;

	UPROPERTY(EditAnywhere, Category = "Obstacle Detection")
	float ObstacleDetectionDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "Obstacle Detection")
	FVector ObstacleDetectionBoxHalfSize = FVector(50.f, 30.f, 70.f);

	// Насколько высоко над точкой контакта искать уступ
	UPROPERTY(EditAnywhere, Category = "Obstacle Detection")
	float LedgeTopOffset = 150.f;       

	// Насколько впереди от стены искать уступ
	UPROPERTY(EditAnywhere, Category = "Obstacle Detection")
	float LedgeForwardOffset = 40.f;    

	
	const float MaxLedgeHeightForVault = 70.f;  // Макс. высота уступа для Vault
	const float MinLedgeHeightForMantle = 80.f; // Мин. высота уступа для Mantle
	const float MaxLedgeHeightForMantle = 200.f;// Макс. высота уступа для Mantle
	
};
