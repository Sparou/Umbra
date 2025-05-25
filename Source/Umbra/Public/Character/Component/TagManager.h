// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "TagManager.generated.h"


struct FGameplayTagContainer;
struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGamplayTagChanged, FGameplayTag, Tag, bool, bAdded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMBRA_API UTagManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTagManager();
	
	UFUNCTION(BlueprintCallable)
	void AddTag(const FGameplayTag& GameplayTag);

	UFUNCTION(BlueprintCallable)
	void RemoveTag(const FGameplayTag& GameplayTag);

	UPROPERTY(BlueprintAssignable)
	FOnGamplayTagChanged OnGameplayTagChanged;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	bool bEnableDebugMode = false;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	FGameplayTagContainer GameplayTags;
	void DrawDebug();
};
