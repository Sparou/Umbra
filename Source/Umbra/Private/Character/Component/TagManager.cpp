// Copyrighted by Vorona Games


#include "Character/Component/TagManager.h"

#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTagManager::UTagManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


}

// Called when the game starts
void UTagManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UTagManager::AddTag(const FGameplayTag& GameplayTag)
{
	GameplayTags.AddTag(GameplayTag);
	OnGameplayTagChanged.Broadcast(GameplayTag, true);
}

void UTagManager::RemoveTag(const FGameplayTag& GameplayTag)
{
	GameplayTags.RemoveTag(GameplayTag);
	OnGameplayTagChanged.Broadcast(GameplayTag, false);
}


// Called every frame
void UTagManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bEnableDebugMode) DrawDebug();
}

void UTagManager::DrawDebug()
{
	if (GEngine)
	{
		for (int32 i = 0; i < GameplayTags.Num(); ++i)
		{
			GEngine->AddOnScreenDebugMessage(i, 0.f, FColor::Blue, GameplayTags.GetByIndex(i).ToString());
		}
	}
}

