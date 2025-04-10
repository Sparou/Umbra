// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraCoreTypes.h"
#include "Character/UmbraBaseCharacter.h"
#include "UmbraPlayerCharacter.generated.h"

class USpringArmComponent;
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
	FStealthKillMontages GetRandomStealthKillMontages(TArray<FStealthKillMontages> KillMontages) const;

	UFUNCTION(BlueprintCallable)
	FStealthKillMontages GetStealthKillMontageForPosition(EStealthKillPosition KillPosition);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components");
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|Animations")
	TArray<FStealthKillMontages> StealthKillMontagesFromBehind;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|Animations")
	TArray<FStealthKillMontages> StealthKillMontagesFromFront;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|Animations")
	TArray<FStealthKillMontages> StealthKillMontagesFromTop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|Animations")
	TArray<FStealthKillMontages> StealthKillMontagesFromLedge;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|Animations")
	TArray<FStealthKillMontages> StealthKillMontagesFromLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stealth|Animations")
	TArray<FStealthKillMontages> StealthKillMontagesFromRight;
	
	virtual void InitAbilityActorInfo() override;
};
