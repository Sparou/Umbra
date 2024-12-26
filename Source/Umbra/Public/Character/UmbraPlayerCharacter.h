// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Character/UmbraBaseCharacter.h"
#include "UmbraPlayerCharacter.generated.h"

USTRUCT(BlueprintType)
struct FStealthKillMontages
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth Kill")
	UAnimMontage* KillerMontage = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth Kill")
	UAnimMontage* VictimMontage = nullptr;
};

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
	AActor* GetOverlappedActorFromInteractionComponent() const;

	UFUNCTION(BlueprintCallable)
	FStealthKillMontages GetRandomStealthKillMontages() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components");
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth")
	TArray<FStealthKillMontages> StealthKillMontages;

	virtual void InitAbilityActorInfo() override;
};
