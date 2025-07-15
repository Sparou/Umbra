// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "InteractionOption.generated.h"

class IInteractionInterface;

USTRUCT(BlueprintType)
struct FInteractionOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<IInteractionInterface> InteractionInterface;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SubText;

	// МЕТОДЫ ВЗАИМОДЕЙСТВИЯ
	//-------------------------------------------------------------

	// 1) Поместить способность на Avatar, который может активировать её по запросу.
	// Этот способ взаимодействия применяется, в случае, если способность взаимодействия влияет на самого игрока.
	// Пример: активировать алтарь, который даст персонажу эффект или новую способность.
	
	/** Способность, которую получает Avatar, когда находится рядом с интерактивным объектом. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> InteractionAbility;
	
	// 2) Позволить интерактивному объекту иметь собственный ASC вместе с Interaction Ability.
	// Этот способ взаимодействия применяется, в случае, если способность взаимодействия влияет на сам объект.
	// Пример: потушить факел, открыть сундук.

	/** Ability SystemComponent, существующий на интерактивном объекте, который мы можем использовать для активации способности. */
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

	/** AbilitySpecHandle, необходимый для активации взаимодействия с интерактивным объектом. */
	UPROPERTY(BlueprintReadWrite)
	FGameplayAbilitySpecHandle TargetInteractionAbilitySpecHandle;

public:
	FORCEINLINE bool operator==(const FInteractionOption& Other) const
	{
		return InteractionInterface == Other.InteractionInterface &&
			InteractionAbility == Other.InteractionAbility &&
			TargetAbilitySystemComponent == Other.TargetAbilitySystemComponent &&
			TargetInteractionAbilitySpecHandle == Other.TargetInteractionAbilitySpecHandle &&
			Text.IdenticalTo(Other.Text) &&
			SubText.IdenticalTo(Other.SubText);
	}

	FORCEINLINE bool operator!=(const FInteractionOption& Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator<(const FInteractionOption& Other) const
	{
		return InteractionInterface.GetInterface() < Other.InteractionInterface.GetInterface();
	}
};
