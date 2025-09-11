#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UmbraGameplayEffectContext.generated.h"

USTRUCT()
struct FUmbraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	FUmbraGameplayEffectContext() : FGameplayEffectContext() {}

	void SetTopHitResult(const FHitResult& InHitResult);

	const FHitResult* GetTopHitResult() const;;

	virtual UScriptStruct* GetScriptStruct() const override { return FUmbraGameplayEffectContext::StaticStruct(); }
	virtual FGameplayEffectContext* Duplicate() const override;
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:

	TSharedPtr<FHitResult> TopHitResult;
};

template<>
struct TStructOpsTypeTraits<FUmbraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FUmbraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true 
	};
};
