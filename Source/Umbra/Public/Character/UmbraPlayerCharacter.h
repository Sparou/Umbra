// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Character/UmbraBaseCharacter.h"
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
	void Fu();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components");
	TObjectPtr<UInteractionComponent> InteractionComponent;

	virtual void InitAbilityActorInfo() override;
};
