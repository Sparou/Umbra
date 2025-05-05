// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Character/UmbraBaseCharacter.h"
#include "Data/StealthKillsData.h"
#include "UmbraPlayerCharacter.generated.h"

class UInteractionComponent;
/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraPlayerCharacter : public AUmbraBaseCharacter
{
	GENERATED_BODY()
public:
	AUmbraPlayerCharacter(const FObjectInitializer& ObjInit);
	
	UFUNCTION(BlueprintCallable)
	FStealthKillMontages GetStealthKillMontagesForPosition(const FGameplayTag& Position);

protected:

	void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UStealthKillsData> StealthKillData;

	virtual void InitAbilityActorInfo() override;
};
