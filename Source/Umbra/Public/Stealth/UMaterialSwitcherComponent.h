// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UMaterialSwitcherComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UMBRA_API UMaterialSwitcherComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMaterialSwitcherComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* DayMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* NightMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials")
	int32 MaterialIndex = 0;

	void SetIsNight(bool bNight);

private:
	void ApplyMaterial(UMaterialInterface* Material);
};


