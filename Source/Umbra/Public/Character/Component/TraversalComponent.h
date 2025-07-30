// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "UmbraGameplayTags.h"
#include "Components/ActorComponent.h"
#include "TraversalComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(UmbraTraversalComponentLog, Log, All);

class UObstacleDetectionSettings;



/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UMBRA_API UTraversalComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTraversalComponent();

protected:
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UObstacleDetectionSettings> ObstacleDetectionSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ClimbObstacleTagName = FName("Climb");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName MantleObstacleTagName = FName("Mantle");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName VaultObstacleTagName = FName("Vault");
	
	bool DetectObstacle();


private:
	
	FVector MoveVector(const FVector& Source, float Offset, const FGameplayTag& DirectionTag, const FRotator& Rotator = FRotator::ZeroRotator) const;

	void FindTraversableActorsInOverlaps(FName& InName, const TArray<FOverlapResult>& OverlapResults);
	
	TWeakObjectPtr<ACharacter> OwnerCharacter;
	
	FUmbraGameplayTags UGT;
};
