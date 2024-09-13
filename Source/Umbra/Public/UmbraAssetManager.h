// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "UmbraAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UUmbraAssetManager& Get();
protected:
	void StartInitialLoading() override;
};
