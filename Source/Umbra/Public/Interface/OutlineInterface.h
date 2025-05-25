// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OutlineInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOutlineInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UMBRA_API IOutlineInterface
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Outline")
	void EnableOutline(int32 StencilValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Outline")
	void DisableOutline();
};
