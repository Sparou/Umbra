// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UmbraGameplayTags.h"
#include "Engine/DataAsset.h"
#include "TraversalActionsData.generated.h"

USTRUCT(BlueprintType)
struct FTraversalActionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* Montage = nullptr;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag InState = FUmbraGameplayTags::Get().Traversal_State_FreeRoam;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag OutState = FUmbraGameplayTags::Get().Traversal_State_FreeRoam;
	
	UPROPERTY(EditAnywhere)
	float Warp1XOffset = 0.f;

	UPROPERTY(EditAnywhere)
	float Warp1ZOffset = 0.f;
	
	UPROPERTY(EditAnywhere)
	float Warp2XOffset = 0.f;

	UPROPERTY(EditAnywhere)
	float Warp2ZOffset = 0.f;

	UPROPERTY(EditAnywhere)
	float Warp3XOffset = 0.f;

	UPROPERTY(EditAnywhere)
	float Warp3ZOffset = 0.f;
};


/**
 * 
 */
UCLASS()
class UMBRA_API UTraversalActionsData : public UDataAsset
{

	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FTraversalActionData> ActionsMap;

	FTraversalActionData FindActionDataByTag(FGameplayTag Action);

};
