#include "Character/GPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/GInputConfig.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/GWeaponActor.h"
#include "Item/GGliderActor.h"
#include "Animation/GAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

AGPlayerCharacter::AGPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	RotationInterpRate = 5.f;
	BaseTurnRate = 40.f;
	BaseLookUpRate = 40.f;

	CurrentViewMode = EViewMode::BackCombatView;
}

void AGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController) == true)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (IsValid(Subsystem) == true)
		{
			Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
		}
	}

	ensureMsgf(PlayerCharacterInputConfig != nullptr, TEXT("Invalid InputConfig"));
	ensureMsgf(PlayerCharacterInputMappingContext != nullptr, TEXT("Invalid InputMappingContext"));
	ensureMsgf(DirectionCurve != nullptr, TEXT("Invalid DirectionCurve"));
	ensureMsgf(WeaponClass != nullptr, TEXT("Invalid WeaponClass"));

}

void AGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	//if (IsValid(AnimInstance) == true)
	//	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f"), AnimInstance->Velocity.Z));

	if (CurrentViewMode == EViewMode::BackCombatView)
	{
		if (GetCharacterMovement()->GetCurrentAcceleration().Length() > 0.f)
		{
			FRotator AccelerationRotationFromX = UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->GetCurrentAcceleration());
			FRotator ControlRotation = GetControlRotation();
			FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(AccelerationRotationFromX, ControlRotation);
			double ApplyYaw = ControlRotation.Yaw + DirectionCurve->GetFloatValue(DeltaRotator.Yaw);

			this->SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(),
				UKismetMathLibrary::MakeRotator(0.0f, 0.0f, ApplyYaw),
				DeltaTime,
				RotationInterpRate
			));
		}
	}
	else if (CurrentViewMode == EViewMode::BackGeneralView)
	{
		if (InputDirectionVector.IsNearlyZero() == false && GetVelocity().IsNearlyZero() == false)
		{
			FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(InputDirectionVector);
			this->SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(),
				TargetRotation,
				DeltaTime,
				RotationInterpRate));
		}
	}


	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance->GetJumpCount() == 3)
	if (IsValid(AnimInstance) == true && AnimInstance->IsFalling() == false)
	{
		if (IsValid(GliderInstance) == true)
		{
			//if (IsValid(GliderInstance->GetUnequipAnimMontage()))// 미사용
			//{
			//	AnimInstance->Montage_Play(GliderInstance->GetUnequipAnimMontage());
			//	AnimInstance->SetWeaponType(EWeaponType::None);
			//}

			GliderInstance->Destroy();
			GliderInstance = nullptr;
		}

		// 글라이더 사용 조건 삭제
		AnimInstance->SetJumpCount(0);

		// 글라이더 미장착 움직임 적용
		GetCharacterMovement()->GravityScale = 1.0f;
		GetCharacterMovement()->AirControl = 0.2f;
	}
}

void AGPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetViewMode(EViewMode::BackCombatView);
}

void AGPlayerCharacter::SetViewMode(EViewMode InViewMode)
{
	if (CurrentViewMode == InViewMode)
	{
		return;
	}

	CurrentViewMode = InViewMode;

	switch (CurrentViewMode)
	{
	case EViewMode::BackCombatView:
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;// ���ÿ� �ü������ ĳ���� ȸ���� ����ȭ
		bUseControllerRotationRoll = false;

		SpringArmComponent->TargetArmLength = 400.f;
		SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);
		// ControlRotation�� Pawn�� ȸ���� ����ȭ,
		// Pawn�� ȸ���� SpringArm�� ȸ���� ����ȭ �����̱⿡ SetRotation()�� ���ǹ�

		SpringArmComponent->bUsePawnControlRotation = true;

		SpringArmComponent->bInheritPitch = true;
		SpringArmComponent->bInheritYaw = true;
		SpringArmComponent->bInheritRoll = false;

		SpringArmComponent->bDoCollisionTest = true;

		//GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
		GetCharacterMovement()->bOrientRotationToMovement = false;// �̵� Ű�� ������ ����ȭ
		GetCharacterMovement()->bUseControllerDesiredRotation = false;

		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;

		GetCharacterMovement()->GravityScale = 1.75f;
		GetCharacterMovement()->MaxAcceleration = 1000.f;
		GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
		GetCharacterMovement()->BrakingFrictionFactor = 1.f;
		GetCharacterMovement()->bUseSeparateBrakingFriction = true;

		GetCharacterMovement()->JumpZVelocity = 700.f;
		GetCharacterMovement()->AirControl = 0.35f;

		break;

	case EViewMode::None:

	case EViewMode::End:

	default:
		break;
	}
}

void AGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent) == true)
	{
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Look, ETriggerEvent::Triggered, this, &ThisClass::InputLook);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Jump, ETriggerEvent::Started, this, &ThisClass::InputJumpStart);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Jump, ETriggerEvent::Completed, this, &ThisClass::InputJumpEnd);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Equip, ETriggerEvent::Started, this, &ThisClass::InputEquip);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->UnEquip, ETriggerEvent::Started, this, &ThisClass::InputUnEquip);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Run, ETriggerEvent::Started, this, &ThisClass::InputRunStart);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Run, ETriggerEvent::Completed, this, &ThisClass::InputRunEnd);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->ChangeView, ETriggerEvent::Started, this, &ThisClass::InputChangeView);
	}
}

void AGPlayerCharacter::InputMove(const FInputActionValue& InValue)
{
	if (IsValid(GetController()) == true)
	{
		FVector2D MovementVector = InValue.Get<FVector2D>();
		ForwardInputValue = MovementVector.X;
		RightInputValue = MovementVector.Y;

		switch (CurrentViewMode)
		{
		case EViewMode::BackCombatView:
		{
			const FRotator ControlRotation = GetController()->GetControlRotation();
			const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);

			const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
			const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

			AddMovementInput(ForwardVector, MovementVector.X);
			AddMovementInput(RightVector, MovementVector.Y);

			InputDirectionVector += ForwardVector * ForwardInputValue;
			InputDirectionVector += RightVector * RightInputValue;
			InputDirectionVector.Normalize();

			break;
		}
		case EViewMode::BackGeneralView:
		{
			const FRotator ControlRotation = GetController()->GetControlRotation();
			const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);

			const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
			const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

			AddMovementInput(ForwardVector, MovementVector.X);
			AddMovementInput(RightVector, MovementVector.Y);

			InputDirectionVector += ForwardVector * ForwardInputValue;
			InputDirectionVector += RightVector * RightInputValue;
			InputDirectionVector.Normalize();

			break;
		}

		case EViewMode::None:

		case EViewMode::End:

		default:
			AddMovementInput(GetActorForwardVector(), MovementVector.X);
			AddMovementInput(GetActorRightVector(), MovementVector.Y);
			break;
		}
	}
}

void AGPlayerCharacter::InputLook(const FInputActionValue& InValue)
{
	if (IsValid(GetController()) == true)
	{
		FVector2D LookVector = InValue.Get<FVector2D>();

		switch (CurrentViewMode)
		{
		case EViewMode::BackCombatView:
			AddControllerYawInput(LookVector.X * BaseTurnRate * GetWorld()->GetDeltaSeconds());
			AddControllerPitchInput(LookVector.Y * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
			break;

		case EViewMode::BackGeneralView:
			AddControllerYawInput(LookVector.X * BaseTurnRate * GetWorld()->GetDeltaSeconds());
			AddControllerPitchInput(LookVector.Y * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
			break;

		case EViewMode::None:

		case EViewMode::End:

		default:
			break;
		}
	}
}

void AGPlayerCharacter::InputJumpStart(const FInputActionValue& InValue)
{
	//ACharacter::Jump();

	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) == true)
	{
		if (AnimInstance->IsFalling() == false)
		{
			AnimInstance->SetJumpCount(0);
		}

		if (AnimInstance->GetJumpCount() < MaxJumpCount)
		{
			uint64 curJumpCount = AnimInstance->GetJumpCount();
			AnimInstance->SetJumpCount(++curJumpCount);

			if (curJumpCount == 1)
			{
				LaunchCharacter(FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity), false, true);
			}
			else
			{
				LaunchCharacter(FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity * 1.5f), false, true);
			}
		}
	}

	if (AnimInstance->GetJumpCount() == 3)
	{
		// 무기 해제
		if (IsValid(AnimInstance) == true && AnimInstance->GetWeaponType() != EWeaponType::None)
		{
			if (IsValid(WeaponInstance) == true)
			{
				TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer = WeaponInstance->GetUnarmedCharacterAnimLayer();
				if (IsValid(UnarmedCharacterAnimLayer) == true)
				{
					GetMesh()->LinkAnimClassLayers(UnarmedCharacterAnimLayer);
				}

				// 무기 해제 애님 몽타주 미사용
				//if (IsValid(WeaponInstance->GetUnequipAnimMontage()))
				{
					//AnimInstance->Montage_Play(WeaponInstance->GetUnequipAnimMontage());
					AnimInstance->SetWeaponType(EWeaponType::None);
				}

				WeaponInstance->Destroy();
				WeaponInstance = nullptr;
			}
		}

		// 글라이더 장착
		FName GliderSocket(TEXT("GliderSocket"));
		if (GetMesh()->DoesSocketExist(GliderSocket) == true && IsValid(GliderInstance) == false)
		{
			GliderInstance = GetWorld()->SpawnActor<AGGliderActor>(GliderClass, FVector::ZeroVector, FRotator::ZeroRotator);
			if (IsValid(GliderInstance) == true)
			{
				GliderInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GliderSocket);
			}

			// 글라이더 장착 애님 몽타주 미사용
			//if (IsValid(GliderInstance->GetEquipAnimMontage()))
			//{
			//	AnimInstance->Montage_Play(GliderInstance->GetEquipAnimMontage());
			//	AnimInstance->SetWeaponType(EWeaponType::None);
			//}

			// 글라이더 움직임 적용
			GetCharacterMovement()->Velocity.Z = 0.0f;
			GetCharacterMovement()->GravityScale = 0.07f;
			GetCharacterMovement()->AirControl = 0.7f;
		}

	}
}

void AGPlayerCharacter::InputJumpEnd(const FInputActionValue& InValue)
{
	// Hop 가능
	//if (GetCharacterMovement()->Velocity.Z > 0.0f)
	//{
	//	GetCharacterMovement()->Velocity = 
	//		UKismetMathLibrary::MakeVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0.0f);
	//}

	// 글라이더 장착 해제
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance->IsFalling() == true)
	{
		if (IsValid(GliderInstance) == true)
		{
			//if (IsValid(GliderInstance->GetUnequipAnimMontage()))// 미사용
			//{
			//	AnimInstance->Montage_Play(GliderInstance->GetUnequipAnimMontage());
			//	AnimInstance->SetWeaponType(EWeaponType::None);
			//}

			GliderInstance->Destroy();
			GliderInstance = nullptr;
		}

		// 글라이더 미장착 움직임 적용
		GetCharacterMovement()->GravityScale = 1.0f;
		GetCharacterMovement()->AirControl = 0.2f;

		// 글라이더 사용 조건 삭제
		AnimInstance->SetJumpCount(0);
	}
}


void AGPlayerCharacter::InputEquip(const FInputActionValue& InValue)
{
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) == true && AnimInstance->GetWeaponType() == EWeaponType::GreatSword)
	{
		return;
	}

	FName WeaponSocket(TEXT("RightHandWeaponSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket) == true && IsValid(WeaponInstance) == false)
	{
		WeaponInstance = GetWorld()->SpawnActor<AGWeaponActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (IsValid(WeaponInstance) == true)
		{
			WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
		}

		TSubclassOf<UAnimInstance> WeaponCharacterAnimLayer = WeaponInstance->GetArmedCharacterAnimLayer();
		if (IsValid(WeaponCharacterAnimLayer) == true)
		{
			GetMesh()->LinkAnimClassLayers(WeaponCharacterAnimLayer);
		}

		if (IsValid(WeaponInstance->GetEquipAnimMontage()))
		{
			AnimInstance->Montage_Play(WeaponInstance->GetEquipAnimMontage());
			AnimInstance->SetWeaponType(EWeaponType::GreatSword);
		}
	}
}

void AGPlayerCharacter::InputUnEquip(const FInputActionValue& InValue)
{
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) == true && AnimInstance->GetWeaponType() == EWeaponType::None)
	{
		return;
	}

	if (IsValid(WeaponInstance) == true)
	{
		TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer = WeaponInstance->GetUnarmedCharacterAnimLayer();
		if (IsValid(UnarmedCharacterAnimLayer) == true)
		{
			GetMesh()->LinkAnimClassLayers(UnarmedCharacterAnimLayer);
		}

		if (IsValid(WeaponInstance->GetUnequipAnimMontage()))
		{
			AnimInstance->Montage_Play(WeaponInstance->GetUnequipAnimMontage());
			AnimInstance->SetWeaponType(EWeaponType::None);
		}

		WeaponInstance->Destroy();
		WeaponInstance = nullptr;
	}
}

void AGPlayerCharacter::InputRunStart(const FInputActionValue& InValue)
{
	do
	{
		UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance) != true)
		{
			break;
		}
		if (AnimInstance->GetLocomotionState() != ELocomotionState::Walk)
		{
			break;
		}

		bIsInputRun = true;

	} while (false);
}

void AGPlayerCharacter::InputRunEnd(const FInputActionValue& InValue)
{
	bIsInputRun = false;
}

void AGPlayerCharacter::InputChangeView(const FInputActionValue& InValue)
{
	switch (CurrentViewMode)
	{
	case EViewMode::BackCombatView:
		CurrentViewMode = EViewMode::BackGeneralView;
		break;

	case EViewMode::BackGeneralView:
		CurrentViewMode = EViewMode::BackCombatView;
		break;

	default:
		break;
	}

	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) == true)
	{
		AnimInstance->SetAnimCurrentViewMode(CurrentViewMode);
	}
}

