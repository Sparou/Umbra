// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UmbraAnimationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UUmbraAnimationInterface : public UInterface
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum ELocomotionStates
{
	Walking,
	Running
};

/**
 * 
 */
class UMBRA_API IUmbraAnimationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="States")
	void UpdateLocomotionState(ELocomotionStates State);
	
};
