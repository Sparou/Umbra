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
	void ApplyMaterialForAllMeshes(UMaterialInterface* MaterialToApply);

	UFUNCTION(BlueprintCallable)
	void RestoreOriginalMaterials();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMaterialInterface* GetInvisibilityMaterial() { return InvisibilityMaterial; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities|Data")
	TObjectPtr<UAssassinationsData> AssassinationsData;
	
	TObjectPtr<UStealthAttributeSet> StealthAttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth")
	TObjectPtr<UStealthComponent> StealthComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth")
	TObjectPtr<UMaterialInterface> InvisibilityMaterial;

private:
	
	TMap<TWeakObjectPtr<UMeshComponent>, TArray<UMaterialInterface*>> MeshComponentsToOriginalMaterials;
};
