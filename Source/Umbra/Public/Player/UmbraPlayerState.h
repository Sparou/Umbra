// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UmbraPlayerState.generated.h"

class UUmbraAttributeSet;
class UUmbraAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class UMBRA_API AUmbraPlayerState : public APlayerState
{

	GENERATED_BODY()
public:

	AUmbraPlayerState();
	
	UUmbraAbilitySystemComponent* GetAbilitySystemComponent() const;
	UUmbraAttributeSet* GetAttributeSet() const;
	int16 GetPlayerLevel() const;

protected:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	
	TObjectPtr<UUmbraAbilitySystemComponent> AbilitySystemComponent;
	TObjectPtr<UUmbraAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, Replicated)
	int16 Level = 1;
};
