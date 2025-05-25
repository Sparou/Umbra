#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/UmbraPlayerCharacter.h"
#include "NPCDetection.generated.h"

class ULightingDetection;

UCLASS()
class UMBRA_API ANPCDetection : public AActor
{
	GENERATED_BODY()

public:
	// Конструктор
	ANPCDetection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Функция для проверки, видит ли NPC игрока
	bool CanSeePlayer();

	// Радиус видимости NPC
	UPROPERTY(EditAnywhere, Category = "NPC Detection")
	float VisionRadius = 1000.f;

	// Минимальный уровень освещенности, при котором NPC может видеть игрока
	UPROPERTY(EditAnywhere, Category = "NPC Detection")
	float MinLightLevel = 50.f;

	// Ссылка на персонажа игрока
	UPROPERTY()
	AUmbraPlayerCharacter* PlayerCharacter;

private:
	// Проверка видимости с учётом коллизий
	bool HasLineOfSight();
};
