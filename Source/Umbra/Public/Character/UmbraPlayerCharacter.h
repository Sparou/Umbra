// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Character/UmbraBaseCharacter.h"
#include "Perception/AISightTargetInterface.h"
#include "UmbraPlayerCharacter.generated.h"

class UStealthAttributeSet;
class UStealthComponent;
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

protected:

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitAbilityActorInfo() override;
	
public:
	
	UFUNCTION(BlueprintCallable)
	UAssassinationsData* GetAssassinationsData();
	
	/** IAISightTargetInterface implementation */
	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(
		const FCanBeSeenFromContext& Context,
		FVector& OutSeenLocation,
		int32& OutNumberOfLoSChecksPerformed,
		int32& OutNumberOfAsyncLosCheckRequested,
		float& OutSightStrength, int32* UserData,
		const FOnPendingVisibilityQueryProcessedDelegate* Delegate) override;

protected:

	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// TObjectPtr<UTraversalComponent> TraversalComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStealthComponent> StealthComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities|Data")
	TObjectPtr<UAssassinationsData> AssassinationsData;

	TObjectPtr<UStealthAttributeSet> StealthAttributeSet;
	
};
