// Copyrighted by Vorona Games


#include "AbilitySystem/Abilities/Shadows/UmbraLineShadowGameplayAbility.h"
#include "AbilitySystem/UmbraAttributeSet.h"
#include "GameFramework/Character.h"
#include "Character/UmbraBaseCharacter.h"
#include "Character/UmbraPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystem/Abilities/Shadows/UmbraLineShadowController.h"
#include "Player/UmbraPlayerController.h"
#include "AbilitySystem/Abilities/Shadows/UmbraShadowController.h"
#include "Player/UmbraForShadowPlayerController.h"


void UUmbraLineShadowGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("ActivateAbility called on %s"), *GetNameSafe(GetAvatarActorFromActorInfo()));
	
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

		FVector Forward = SourceCharacter->GetActorForwardVector();
		FVector Right = SourceCharacter->GetActorRightVector();

		if (NumClones % 2 == 1)
		{
			NumClones -= 1;
		}
		int32 HalfCount = NumClones / 2;

		for (int32 i = 0; i < NumClones; ++i)
		{
			int32 OffsetIndex = i - HalfCount;

			// Если NumClones чётное, пропускаем 0, чтобы не спавнить клона прямо на игроке
			if (NumClones % 2 == 0 && OffsetIndex >= 0)
			{
				OffsetIndex += 1;
			}

			FVector Offset = Right * Spacing * OffsetIndex;
			FVector TargetLocation = Origin + Offset;
			FRotator SpawnRotation = Forward.Rotation();
			
			AUmbraBaseCharacter* ClonedCharacter = GetWorld()->SpawnActor<AUmbraBaseCharacter>(
				LineShadowCharacterClass, Origin, SpawnRotation, SpawnParams
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

				AUmbraLineShadowController* ShadowController = GetWorld()->SpawnActor<AUmbraLineShadowController>(
					AUmbraLineShadowController::StaticClass(),
					ClonedCharacter->GetActorLocation(),
					ClonedCharacter->GetActorRotation(),
					ControllerSpawnParams
				);
				
				ShadowController->MasterCharacter = Cast<AUmbraPlayerCharacter>(SourceCharacter);
				ShadowController->FixedOffset = 200.f * OffsetIndex;

				if (ShadowController)
				{
					ShadowController->Possess(ClonedCharacter);
					ShadowController->SetControlRotation(ClonedCharacter->GetActorRotation());
					ClonedCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					
					// Добавляем в список в контроллере
					if (UmbraPC)
					{
						UmbraPC->LineShadowControllers.Add(ShadowController);
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

void UUmbraLineShadowGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController);
	if (!PC && ActorInfo->AvatarActor.IsValid())
	{
		PC = Cast<APlayerController>(ActorInfo->AvatarActor->GetInstigatorController());
	}
	
	AUmbraForShadowPlayerController* UmbraPC = Cast<AUmbraForShadowPlayerController>(PC);
	
	if (UmbraPC)
	{
		for (AUmbraLineShadowController* Controller : UmbraPC->LineShadowControllers)
		{
			if (Controller)
			{
				Controller->Destroy();
			}
		}
		UmbraPC->LineShadowControllers.Empty();
	}
}
