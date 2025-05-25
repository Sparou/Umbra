// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraAlarmBell.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBellRang, FVector, BellLocation);

UCLASS()
class UMBRA_API AUmbraAlarmBell : public AActor
{
	GENERATED_BODY()
	
public:	
	AUmbraAlarmBell();

	UPROPERTY(BlueprintAssignable, Category = "Alarm")
	FOnBellRang OnBellRang;

	void RingBell() const;

protected:
	virtual void BeginPlay() override;
};
