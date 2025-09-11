
#include "AbilitySystem/GameplayEffect/UmbraGameplayEffectContext.h"


void FUmbraGameplayEffectContext::SetTopHitResult(const FHitResult& InHitResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Inside SetTopHitResult, 'this' pointer is %p"), this)
	TopHitResult = MakeShared<FHitResult>(InHitResult);
}

const FHitResult* FUmbraGameplayEffectContext::GetTopHitResult() const
{
	if (TopHitResult.IsValid())
	{
		return TopHitResult.Get();
	}

	return nullptr;
}

FGameplayEffectContext* FUmbraGameplayEffectContext::Duplicate() const
{
	FUmbraGameplayEffectContext* NewContext = new FUmbraGameplayEffectContext();
	*NewContext = *this;

	if (TopHitResult.IsValid())
	{
		NewContext->TopHitResult = MakeShared<FHitResult>(*TopHitResult);
	}

	return NewContext;
}

bool FUmbraGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
		bool bHasTopHitResult = TopHitResult.IsValid();
		Ar << bHasTopHitResult;
		if (bHasTopHitResult)
		{
			TopHitResult->NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else
	{
		bool bHasTopHitResult;
		Ar << bHasTopHitResult;
		if (bHasTopHitResult)
		{
			TopHitResult = MakeShared<FHitResult>(*TopHitResult);
			TopHitResult->NetSerialize(Ar, Map, bOutSuccess);
		}
	}

	bOutSuccess = true;
	return true;
}
