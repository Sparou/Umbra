// Copyrighted by Vorona Games


#include "UmbraAssetManager.h"
#include "UmbraGameplayTags.h"

UUmbraAssetManager& UUmbraAssetManager::Get()
{
	check(GEngine);
	UUmbraAssetManager* UmbraAssetManager = Cast<UUmbraAssetManager>(GEngine->AssetManager);
	return *UmbraAssetManager;
}

void UUmbraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FUmbraGameplayTags::InitializeNativeGameplayTags();
}
