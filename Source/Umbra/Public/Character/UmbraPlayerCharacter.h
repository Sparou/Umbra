// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Character/UmbraBaseCharacter.h"
#include "Stealth/LightingDetection.h"
#include "Blueprint/UserWidget.h"
#include "Perception/AISightTargetInterface.h"
#include "UmbraPlayerCharacter.generated.h"

class UAssassinationsData;
class UInteractionComponent;
class UTraversalComponent;

/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraPlayerCharacter : public AUmbraBaseCharacter, public IAISightTargetInterface 
{
	GENERATED_BODY()
public:
	AUmbraPlayerCharacter(const FObjectInitializer& ObjInit);
	
	UFUNCTION(BlueprintCallable)
	UAssassinationsData* GetAssassinationsData();
	
	const ULightingDetection* GetLightingDetector() const;

	/** IAISightTargetInterface implementation */
	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(
		const FCanBeSeenFromContext& Context,
		FVector& OutSeenLocation,
		int32& OutNumberOfLoSChecksPerformed,
		int32& OutNumberOfAsyncLosCheckRequested,
		float& OutSightStrength, int32* UserData,
		const FOnPendingVisibilityQueryProcessedDelegate* Delegate) override;

protected:

	void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTraversalComponent> TraversalComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UAssassinationsData> AssassinationsData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Sight",
	meta = (ToolTip = "Table of player visibility thresholds depending on the distance to the AI"))
	TObjectPtr<UCurveTable> ThresholdOfVisibilityFromDistanceSquaredTable;
	
	virtual void InitAbilityActorInfo() override;
	
private:
	/** Компонент определения освещённости */
	UPROPERTY(VisibleAnywhere, Category = "Lighting")
	ULightingDetection* LightingDetector;
	
	/** Наш виджет-класс полоски */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LightWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Tag Name")
	FName InvisibilityTagName = "Ability.Invisibility";
};
