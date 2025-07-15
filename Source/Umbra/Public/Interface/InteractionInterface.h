// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilitySystem/Interaction/InteractionOption.h"
#include "AbilitySystem/Interaction/InteractionQuery.h"
#include "InteractionInterface.generated.h"


/** Вспомогательный класс, упрощающий процесс сбора доступных взаимодействий с объекта. */
class FInteractionOptionBuilder
{
public:

	FInteractionOptionBuilder(TScriptInterface<IInteractionInterface> InteractionInterfaceScope, TArray<FInteractionOption>& InteractionOptions)
		: Scope(InteractionInterfaceScope), Options(InteractionOptions)
	{
		
	}

	void AddInteractionOption(const FInteractionOption& Option)
	{
		FInteractionOption& OptionEntry = Options.Add_GetRef(Option);
		OptionEntry.InteractionInterface = Scope;
	}
	
private:
	TScriptInterface<IInteractionInterface> Scope;
	TArray<FInteractionOption>& Options;
};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Интерфейс, используемый для взаимодействия с объектом.
 * Дополнительно смотрите: `InteractionAbility.h` и InteractionOption.h.
 */
class UMBRA_API IInteractionInterface
{
	GENERATED_BODY()

public:
	
	FInteractionOption GetInteractionOption() const;
	virtual void GatherInteractionOption(const FInteractionQuery& Query, FInteractionOptionBuilder& Builder) = 0;
	virtual void CustomizeInteractionEventData(const FGameplayTag& EventTag, FGameplayEventData& InOutEventData) { }
};
