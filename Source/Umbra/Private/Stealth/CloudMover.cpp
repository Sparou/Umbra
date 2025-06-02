// Copyrighted by Vorona Games


#include "Stealth/CloudMover.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACloudMover::ACloudMover()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and attach mesh component
    CloudMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CloudMesh"));
    RootComponent = CloudMesh;

    // Default values
    Speed = 200.0f;
    Radius = 1000.0f;
    RotationInterpSpeed = 5.0f; // degrees per second
}

// Called when the game starts or when spawned
void ACloudMover::BeginPlay()
{
    Super::BeginPlay();
    ChooseNewTarget();
}

// Called every frame
void ACloudMover::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector CurrentLocation = GetActorLocation();
    FVector Direction = TargetPoint - CurrentLocation;
    Direction.Z = 0; // ignore vertical component
    float Distance = Direction.Size();

    // If reached or too close, pick a new target
    if (Distance < KINDA_SMALL_NUMBER)
    {
        ChooseNewTarget();
        return;
    }

    // Move towards target
    Direction.Normalize();
    FVector NewLocation = CurrentLocation + Direction * Speed * DeltaTime;
    SetActorLocation(NewLocation);

    // Calculate desired rotation to face movement direction
    FRotator DesiredRotation = Direction.Rotation();
    FRotator CurrentRotation = GetActorRotation();

    // Interpolate rotation towards desired
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, RotationInterpSpeed);
    SetActorRotation(NewRotation);

    // If close to target, choose new
    if (FVector::DistXY(NewLocation, TargetPoint) < 50.0f)
    {
        ChooseNewTarget();
    }
}

void ACloudMover::ChooseNewTarget()
{
    // Generate random point in circle on X-Y plane
    float Angle = UKismetMathLibrary::RandomFloatInRange(0.0f, 2 * PI);
    float Dist = UKismetMathLibrary::RandomFloatInRange(0.0f, Radius);
    FVector Origin = GetActorLocation();

    // Keep current Z
    TargetPoint = FVector(
        Origin.X + FMath::Cos(Angle) * Dist,
        Origin.Y + FMath::Sin(Angle) * Dist,
        Origin.Z
    );
}

