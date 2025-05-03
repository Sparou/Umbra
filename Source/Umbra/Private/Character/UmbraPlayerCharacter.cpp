#include "Character/UmbraPlayerCharacter.h"
#include "Character/Component/InteractionComponent.h"
#include "AbilitySystem/UmbraAttributeSet.h"
#include "Stealth/LightingDetection.h"
#include "Blueprint/UserWidget.h"
#include "Stealth/LightLevelIndicator.h"

// --------------------------------------------------
// Конструктор без параметра FObjectInitializer
AUmbraPlayerCharacter::AUmbraPlayerCharacter()
    : Super()  // явно вызываем базовый конструктор
{
    AttributeSet = CreateDefaultSubobject<UUmbraAttributeSet>(TEXT("AttributeSet"));
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
    LightingDetector = CreateDefaultSubobject<ULightingDetection>(TEXT("LightingDetection"));
}

// --------------------------------------------------
void AUmbraPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (LightWidgetClass)
    {
        // Создаём и показываем наш C++-виджет
        ULightLevelIndicator* Widget = CreateWidget<ULightLevelIndicator>(GetWorld(), LightWidgetClass);
        if (Widget)
        {
            Widget->AddToViewport();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create LightLevelIndicator widget"));
        }
    }
}

void AUmbraPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Вся остальная логика тика остаётся здесь
}

AActor* AUmbraPlayerCharacter::GetOverlappedActorFromInteractionComponent() const
{
    return InteractionComponent->GetCurrentTarget();
}

FStealthKillMontages AUmbraPlayerCharacter::GetRandomStealthKillMontages(TArray<FStealthKillMontages> KillMontages) const
{
    return KillMontages.Num() > 0
        ? KillMontages[FMath::RandRange(0, KillMontages.Num() - 1)]
        : FStealthKillMontages();
}

FStealthKillMontages AUmbraPlayerCharacter::GetStealthKillMontageForPosition(EStealthKillPosition KillPosition)
{
    switch (KillPosition)
    {
        case EStealthKillPosition::Behind:
            return GetRandomStealthKillMontages(StealthKillMontagesFromBehind);
        // … остальные case’ы …
        default:
            UE_LOG(LogTemp, Warning, TEXT("Unknown KillPosition: %d"), static_cast<int32>(KillPosition));
            return FStealthKillMontages();
    }
}

void AUmbraPlayerCharacter::InitAbilityActorInfo()
{
    Super::InitAbilityActorInfo();
}
