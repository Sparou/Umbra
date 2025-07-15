// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "InteractionQuery.generated.h"

USTRUCT(BlueprintType)
struct FInteractionQuery
{
	GENERATED_BODY()

public:

	/** Actor, который посылает запрос на взаимодействие с интерактивным объектом. */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> RequestingActor;

	/** Позволяет указывать Controller, который инициировал взаимодействие. */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AController> RequestingController;

	/** Опциональный UObject, который может содержать дополнительую информацию о взаимодейсвтии. */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UObject> OptionalObjectData;
};
