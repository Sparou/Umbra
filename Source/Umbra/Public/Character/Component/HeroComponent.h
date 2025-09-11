// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "HeroComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(HeroComponentLog, Log, All);

class UInputMappingContext;
class UUmbraInputConfig;



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UMBRA_API UHeroComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UHeroComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

protected:
	
	TObjectPtr<UUmbraInputConfig> UmbraInputConfig;
	TObjectPtr<UInputMappingContext> DefaultMapping;
	TObjectPtr<UInputMappingContext> ClimbMapping;
};
