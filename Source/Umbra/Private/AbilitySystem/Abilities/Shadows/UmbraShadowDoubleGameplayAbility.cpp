// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Shadows/UmbraShadowDoubleGameplayAbility.h"

#include "AbilitySystem/UmbraAttributeSet.h"
#include "GameFramework/Character.h"
#include "Character/UmbraBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystem/Abilities/Shadows/UmbraShadowController.h"
#include "Player/UmbraForShadowPlayerController.h"


void UUmbraShadowDoubleGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
	ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	if (!SourceCharacter)
	{
		return;
	}
	

	APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController);
	if (!PC && ActorInfo->AvatarActor.IsValid())
	{
		PC = Cast<APlayerController>(ActorInfo->AvatarActor->GetInstigatorController());
	}

	AUmbraForShadowPlayerController* UmbraPC = Cast<AUmbraForShadowPlayerController>(PC);

	if (UmbraPC)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = SourceCharacter->GetOwner();
		SpawnParams.Instigator = SourceCharacter;
		
		FVector Origin = SourceCharacter->GetActorLocation();

		// Отключаем движение
		SourceCharacter->GetCharacterMovement()->DisableMovement();

		// Создаём таймер на восстановление
		SourceCharacter->GetWorldTimerManager().SetTimer(
			MovementEnableTimer,
			[SourceCharacter]()
			{
				if (SourceCharacter && SourceCharacter->GetCharacterMovement())
				{
					SourceCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				}
			},
			TimeBlockCharac,  // Длительность блокировки
			false  // Не повторять
		);

		for (int32 i = 0; i < NumClones; ++i)
		{
			float Angle = (2 * PI / NumClones) * i;
			FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0) * Radius;
			FVector TargetLocation = Origin + Offset;
			FRotator SpawnRotation = (Origin - TargetLocation).Rotation();

			AUmbraBaseCharacter* ClonedCharacter = GetWorld()->SpawnActor<AUmbraBaseCharacter>(
				ShadowCharacterClass, Origin, SpawnRotation, SpawnParams
			);
			ClonedCharacter->IsShadow = IsShadow;
			ClonedCharacter->SetInvisibility(IsShadow);
			
			UCapsuleComponent* Capsule = ClonedCharacter->GetCapsuleComponent();
			if (Capsule && IsShadow)
			{
				Capsule->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
			}

			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = ClonedCharacter;
			LatentInfo.ExecutionFunction = FName(TEXT("OnCloneMoveFinished")); // если вы хотите коллбэк по завершению
			LatentInfo.Linkage = 0;
			LatentInfo.UUID = __LINE__; // уникальный идентификатор действия

			UKismetSystemLibrary::MoveComponentTo(
				Capsule,
				TargetLocation,
				SpawnRotation,
				false,
				false,
				0.5f,
				false,
				EMoveComponentAction::Type::Move,
				LatentInfo
			);

			if (ClonedCharacter)
			{
				FActorSpawnParameters ControllerSpawnParams;
				ControllerSpawnParams.Owner = ClonedCharacter;
				ControllerSpawnParams.Instigator = ClonedCharacter;

				AUmbraShadowController* ShadowController = GetWorld()->SpawnActor<AUmbraShadowController>(
					AUmbraShadowController::StaticClass(),
					ClonedCharacter->GetActorLocation(),
					ClonedCharacter->GetActorRotation(),
					ControllerSpawnParams
				);

				if (ShadowController)
				{
					ShadowController->Possess(ClonedCharacter);
					ShadowController->SetControlRotation(ClonedCharacter->GetActorRotation());
					ClonedCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

					// Добавляем в список в контроллере
					if (UmbraPC)
					{
						UmbraPC->ShadowControllers.Add(ShadowController);
					}

					UE_LOG(LogTemp, Warning, TEXT("Shadow clone %d possessed"), i + 1);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get UmbraPlayerController"));
	}
}

void UUmbraShadowDoubleGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AUmbraForShadowPlayerController* UmbraPC = Cast<AUmbraForShadowPlayerController>(ActorInfo->PlayerController);
	if (!UmbraPC && ActorInfo->AvatarActor.IsValid())
	{
		UmbraPC = Cast<AUmbraForShadowPlayerController>(ActorInfo->AvatarActor->GetInstigatorController());
	}

	if (UmbraPC)
	{
		for (AUmbraShadowController* Controller : UmbraPC->ShadowControllers)
		{
			if (Controller)
			{
				Controller->Destroy();
			}
		}
		UmbraPC->ShadowControllers.Empty();
	}
}
