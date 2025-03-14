// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TraversalInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UTraversalInterface : public UInterface
{
	GENERATED_BODY()
};

struct FGameplayTag;

/**
 * 
 */
class UMBRA_API ITraversalInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
	bool SetTraversalState(const FGameplayTag& NewTraversalState);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
	bool SetTraversalAction(const FGameplayTag& NewTraversalAction);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
	bool SetClimbStyle(const FGameplayTag& NewClimbStyle);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
	bool SetClimbDirection(const FGameplayTag& NewClimbDirection);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
	bool SetLeftHandClimbLocation(const FVector& NewLeftHandClimbLocation);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
	bool SetRightHandClimbLocation(const FVector& NewRightHandClimbLocation);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
	bool SetLeftHandClimbRotation(const FRotator& NewLeftHandClimbRotation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
	bool SetRightHandClimbRotation(const FRotator& NewRightHandClimbRotation);
};
