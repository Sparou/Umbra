// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "UmbraEffectActor.generated.h"


class UGameplayEffect;
class UAbilitySystemComponent;
struct FActiveGameplayEffectHandle;

DECLARE_LOG_CATEGORY_EXTERN(UmbraEffectActorLog, Log, All);

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnBeginOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};

UENUM(BlueprintType)
enum class EEffectTargetTypePolicy : uint8
{
	ApplyOnPlayers,
	ApplyOnEnemies,
	ApplyOnBoth,
};

UENUM(BlueprintType)
enum class EEffectActorDestructionPolicy : uint8
{
	WhenAllEffectsApplied,
	OnActorBeginOverlap,
	OnActorEndOverlap,
	DoNotDestroy
};

USTRUCT(BlueprintType)
struct FOverlapEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> GameplayEffectClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EffectLevel = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectApplicationPolicy ApplicationPolicy = EEffectApplicationPolicy::ApplyOnBeginOverlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectRemovalPolicy RemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectTargetTypePolicy TargetTypePolicy = EEffectTargetTypePolicy::ApplyOnBoth;

	bool bIsApplied = false;
};

USTRUCT()
struct FAppliedOverlapEffectData
{
	GENERATED_BODY()

	FActiveGameplayEffectHandle GameplayEffectHandle;
	EEffectRemovalPolicy RemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	FORCEINLINE bool operator== (const FAppliedOverlapEffectData& Other) const
	{
		return GameplayEffectHandle == Other.GameplayEffectHandle;
	}
};



/**
 * Стандартный для этого проекта класс, используемый для создания акторов, способных применять эффекты к другим акторам.
 */
UCLASS()
class UMBRA_API AUmbraEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AUmbraEffectActor();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, FOverlapEffectData& EffectData);

	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectActorDestructionPolicy DestructionPolicy = EEffectActorDestructionPolicy::DoNotDestroy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FOverlapEffectData> OverlapEffects;

private:

	TMap<TWeakObjectPtr<AActor>, TArray<FAppliedOverlapEffectData>> ActiveEffects;
	
	void RemoveActiveEffectFromTarget(AActor* TargetActor);
	void CleanupInvalidTargets();
	bool CheckAllEffectsAreApplied();
};
