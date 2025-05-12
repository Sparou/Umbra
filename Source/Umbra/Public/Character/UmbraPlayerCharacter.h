// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Character/UmbraBaseCharacter.h"
#include "UmbraPlayerCharacter.generated.h"

class UAssassinationsData;
class UInteractionComponent;
class UTraversalComponent;

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
	UAssassinationsData* GetAssassinationsData();

protected:

	void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTraversalComponent> TraversalComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UAssassinationsData> AssassinationsData;

	virtual void InitAbilityActorInfo() override;
};
