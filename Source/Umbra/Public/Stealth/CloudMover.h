// Copyrighted by Vorona Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CloudMover.generated.h"

UCLASS()
class UMBRA_API ACloudMover : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ACloudMover();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Mesh component for the cloud
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cloud")
    UStaticMeshComponent* CloudMesh;

    // Movement speed (units per second)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Speed;

    // Radius of horizontal movement area
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Radius;

    // Rotation interpolation speed (degrees per second)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RotationInterpSpeed;

private:
    // Current target point
    FVector TargetPoint;

    // Choose a new random target within the horizontal radius
    void ChooseNewTarget();
};