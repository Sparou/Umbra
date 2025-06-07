// Copyrighted by Vorona Games


#include "UI/UmbraMainWidget.h"

#include "Components/PanelWidget.h"


void UUmbraMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!PausePanel || !GameplayPanel) return;

	PausePanel->SetVisibility(bIsPause ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	GameplayPanel->SetVisibility(bIsPause ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}


void UUmbraMainWidget::SwitchPauseMode()
{
	bIsPause = !bIsPause;
	
	if (!PausePanel || !GameplayPanel) return;

	PausePanel->SetVisibility(bIsPause ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	GameplayPanel->SetVisibility(bIsPause ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}
