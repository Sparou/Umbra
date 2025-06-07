// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UmbraMainMenuWidget.generated.h"

class AActor;
class APawn;
class UButton;

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu")
	TArray<TSubclassOf<AActor>> MenuActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu")
	TArray<TSubclassOf<APawn>> CorrespondingPawns;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	FVector SpawnLocation = FVector(0.f, 0.f, 100.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	FRotator SpawnRotation = FRotator::ZeroRotator;

private:

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void SpawnCurrentActor();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void UpdateSelection(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void SaveChoose();

	int32 CurrentIndex = 0;
	AActor* SpawnedActor = nullptr;
};
