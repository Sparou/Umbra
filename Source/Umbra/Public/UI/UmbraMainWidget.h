// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UmbraMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraMainWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	bool bIsPause = false;
	
	UFUNCTION(BlueprintCallable)
	void SwitchPauseMode();

protected:	
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* PausePanel;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* GameplayPanel;
};
