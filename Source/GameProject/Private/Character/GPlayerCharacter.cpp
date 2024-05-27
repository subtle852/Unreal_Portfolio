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
	BaseTurnRate = 50.f;
	BaseLookUpRate = 50.f;

	CurrentViewMode = EViewMode::BackCombatView;

	GliderAirControl = 0.7f;

	bIsFliping = false;
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

	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) == true)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnMontageEnded);
	}

	ensureMsgf(PlayerCharacterInputConfig != nullptr, TEXT("Invalid InputConfig"));
	ensureMsgf(PlayerCharacterInputMappingContext != nullptr, TEXT("Invalid InputMappingContext"));
	ensureMsgf(DirectionCurve != nullptr, TEXT("Invalid DirectionCurve"));
	ensureMsgf(WeaponClass != nullptr, TEXT("Invalid WeaponClass"));
	ensureMsgf(GliderClass != nullptr, TEXT("Invalid GliderClass"));
	ensureMsgf(JumpFlipMontage != nullptr, TEXT("Invalid JumpFlipMontage"));

}

void AGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	//if (IsValid(AnimInstance) == true)
	//	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f"), AnimInstance->Velocity.Z));

	// 시점(움직임)관련 부분
	{
		UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance) == true)
		{
			if (CurrentViewMode == EViewMode::BackCombatView || AnimInstance->IsGliding() == true)
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
			else if (CurrentViewMode == EViewMode::BackGeneralView && AnimInstance->IsGliding() == false)
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
		}
	}

	// 글라이딩 관련 부분
	{
		UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance) == true)
		{
			// Gliding ViewMode(움직임) 설정은 AnimInstance에서 관리 중

			// 글라이더 제거 및 글라이더 미착용 관련 변수 설정 
			// (추후 이 부분은 OnLand 부분으로 이동 예정)
			if (AnimInstance->IsFalling() == false)
			{
				if (IsValid(GliderInstance) == true)
				{
					// 글라이더 탈착 애님 몽타주 미사용
					//if (IsValid(GliderInstance->GetUnequipAnimMontage()))
					//{
					//	AnimInstance->Montage_Play(GliderInstance->GetUnequipAnimMontage());
					//	AnimInstance->SetWeaponType(EWeaponType::None);
					//}

					GliderInstance->Destroy();
					GliderInstance = nullptr;
				}

				AnimInstance->SetGliding(false);

				AnimInstance->SetJumpCount(0);

				// 글라이더 미장착 움직임 적용
				GetCharacterMovement()->GravityScale = 1.75f;
				GetCharacterMovement()->AirControl = 0.35f;
			}

			// 3단 점프 후, 낙하 시작 시점에 글라이더 착용 움직임 설정
			if (AnimInstance->IsFalling() == true && AnimInstance->IsGliding() == true
				&& AnimInstance->GetJumpCount() == MaxJumpCount)
			{
				// 3단 점프를 하는 경우에 InputStart 내부에서
				// 아래 코드 부분이 호출되면 중력으로인해 위로 붕뜨는 문제 발생
				// 따라서 InputStart 내부가 아닌 여기에 작성

				if (GetCharacterMovement()->Velocity.Z <= 0.0f)
				{
					//GetCharacterMovement()->Velocity.Z = 0.0f;
					GetCharacterMovement()->GravityScale = 0.05f;
					GetCharacterMovement()->AirControl = GliderAirControl;
				}
			}

			// 글라이딩 조작에 따른 액터가 회전하는 부분
			if (AnimInstance->IsFalling() == true && AnimInstance->IsGliding() == true)
			{
				if (InputDirectionVector.IsNearlyZero() == false)
				{
					// 무브먼트 Input에 따른 글라이딩 상태 회전 적용
					const float MaxAngle = 45.0f;
					const float InterpRate = 3.0f;

					float TargetRoll = RightInputValue * MaxAngle;
					float TargetPitch = -ForwardInputValue * MaxAngle;
					if (CurrentViewMode == EViewMode::BackGeneralView && AnimInstance->IsGliding() == false)
					{
						// BackGeneralView인 경우에는 Backward 움직임 키를 누르더라도
						// Forward와 동일하게 동작하기위한 부분
						if (ForwardInputValue < 0)
							TargetPitch = -(TargetPitch);
					}

					FRotator TargetRotation = FRotator(TargetPitch, GetActorRotation().Yaw, TargetRoll);
					FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, InterpRate);
					SetActorRotation(NewRotation);

					// (Deprecated)
					// 회전된 상태에서 바닥에 부딪혀도 bIsFalling이 초기화 되지않기에
					// 캡슐 컴포넌트의 회전은 고정시키는 방식
					//FRotator OriginalRotation = GetCapsuleComponent()->GetComponentRotation();
					//FRotator FixedRotation = FRotator(0.f, OriginalRotation.Yaw, 0.f);
					//GetCapsuleComponent()->SetWorldRotation(FixedRotation);
				}
				else
				{
					// Movement Input이 없는 경우에 원 상태로 회전시키는 부분
					const float InterpRate = 3.0f;
					FRotator TargetRotation = FRotator(0.0f, GetActorRotation().Yaw, 0.0f);
					FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, InterpRate);
					SetActorRotation(NewRotation);
				}
			}

			// 바닥과 닿았는지 충돌 체크하는 부분
			if (AnimInstance->IsFalling() == true)
			{
				// 회전된 상태에서 바닥에 부딪혀도 bIsFalling이 초기화 되지않기에
				// 충돌체크를 통해 물체(바닥)와의 충돌 여부를 확인하고
				// 충돌한 경우에 캐릭 pitch, roll 로테이션 초기화 진행
				// 해당 부분은 Tick이 아닌 타이머를 통해 확인하는 것이 좋음

				if (GetActorRotation().Pitch != 0.0f || GetActorRotation().Roll != 0.0f)
				{
					const float Range = 110.f;

					FHitResult HitResult;
					FCollisionQueryParams Params(NAME_None, false, this);
					Params.AddIgnoredActor(this);

					bool bResult = GetWorld()->LineTraceSingleByChannel(
						HitResult,
						GetActorLocation(),
						GetActorLocation() + Range * -GetActorUpVector(),
						//ECC_GameTraceChannel2,
						ECC_Pawn,
						Params
					);

					if (true == bResult)
					{
						if (true == ::IsValid(HitResult.GetActor()))
						{
							UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));
						}

						FRotator TargetRotation = FRotator(0.0f, GetActorRotation().Yaw, 0.0f);
						SetActorRotation(TargetRotation);
					}
				}
			}
		}
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

	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) == true)
	{
		AnimInstance->SetAnimCurrentViewMode(CurrentViewMode);
	}

	switch (CurrentViewMode)
	{
	case EViewMode::BackCombatView:
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;

		SpringArmComponent->TargetArmLength = 400.f;
		SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);

		SpringArmComponent->bUsePawnControlRotation = true;

		SpringArmComponent->bInheritPitch = true;
		SpringArmComponent->bInheritYaw = true;
		SpringArmComponent->bInheritRoll = false;

		SpringArmComponent->bDoCollisionTest = true;

		//GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
		GetCharacterMovement()->bOrientRotationToMovement = false;
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

void AGPlayerCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage->GetName().Equals(TEXT("RPG-Character_Unarmed-Jump-Flip_Anim_Montage"), ESearchCase::IgnoreCase) == true)
	{
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		bIsFliping = false;
	}
}

void AGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent) == true)
	{
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Move, ETriggerEvent::Completed, this, &ThisClass::InputMoveEnd);
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
			[[fallthrough]]

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

void AGPlayerCharacter::InputMoveEnd(const FInputActionValue& InValue)
{
	// 인풋 초기화
	InputDirectionVector = FVector::ZeroVector;
	RightInputValue = 0.0f;
	ForwardInputValue = 0.0f;
}

void AGPlayerCharacter::InputLook(const FInputActionValue& InValue)
{
	if (IsValid(GetController()) == true)
	{
		FVector2D LookVector = InValue.Get<FVector2D>();

		switch (CurrentViewMode)
		{
		case EViewMode::BackCombatView:
			[[fallthrough]]

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
	ensureMsgf(IsValid(AnimInstance) != false, TEXT("Invalid AnimInstance"));

	// 기본적인 점프 관련 부분
	{
		// 처음 점프할 때 Count 초기화 예외 처리 
		if (AnimInstance->IsFalling() == false)
		{
			AnimInstance->SetJumpCount(0);
		}

		// 점프 카운트 ++
		uint64 curJumpCount = AnimInstance->GetJumpCount();
		curJumpCount++;
		AnimInstance->SetJumpCount(curJumpCount);

		// LaunchCharacter
		if (AnimInstance->GetJumpCount() <= MaxJumpCount)
		{
			float JumpZVelocityMultiplier = 1.0f;

			if (curJumpCount == 1)// 1단 점프
			{
				LaunchCharacter(FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity * JumpZVelocityMultiplier), false, true);
			}
			else if (curJumpCount == 2)// 2단 점프
			{
				if (IsValid(JumpFlipMontage) == true && bIsFliping == false)
				{
					AnimInstance->PlayAnimMontage(JumpFlipMontage);// 애님 몽타주 실행
					bIsFliping = true;
				}

				JumpZVelocityMultiplier = 1.5f;// 중력으로 인해 더 힘을 줘야 함
				LaunchCharacter(FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity * JumpZVelocityMultiplier), false, true);
			}
			else if(curJumpCount == 3)// 3단 점프
			{
				FRotator Rotator = GetActorForwardVector().Rotation();
				Rotator.Pitch += 45.0f;// 각도 위로 Pitch 회전
				FVector DiagonalVector = FRotationMatrix(Rotator).GetUnitAxis(EAxis::X);
				LaunchCharacter(DiagonalVector * 2000.0f, false, true);
			}
		}
	}

	// 3단 점프 및 글라이더 관련 부분
	{
		// 3단 점프 혹은 하강 중 글라이더 해제한 후 다시 착용하는 경우
		if (AnimInstance->GetJumpCount() >= MaxJumpCount && AnimInstance->IsGliding() == false)
		{
			AnimInstance->SetGliding(true);

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
			}
		}

		// 글라이더 해제한 후 다시 착용하는 경우 움직임 설정
		if (AnimInstance->GetJumpCount() > MaxJumpCount && AnimInstance->IsGliding() == true
			&& GetCharacterMovement()->Velocity.Z <= 0.0f)
		{
			GetCharacterMovement()->Velocity.Z = 0.0f;
			GetCharacterMovement()->GravityScale = 0.05f;
			GetCharacterMovement()->AirControl = GliderAirControl;
		}
	}
}

void AGPlayerCharacter::InputJumpEnd(const FInputActionValue& InValue)
{
	// Hop 기능 (Deprecated)
	//if (GetCharacterMovement()->Velocity.Z > 0.0f)
	//{
	//	GetCharacterMovement()->Velocity = 
	//		UKismetMathLibrary::MakeVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0.0f);
	//}

	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance) != false, TEXT("Invalid AnimInstance"));

	// 글라이딩 중이라면, 글라이더 장착 해제
	if (AnimInstance->IsGliding() == true && AnimInstance->IsFalling() == true)
	{
		if (IsValid(GliderInstance) == true)
		{
			// 글라이더 장착 애님 몽타주 미사용
			//if (IsValid(GliderInstance->GetUnequipAnimMontage()))
			//{
			//	AnimInstance->Montage_Play(GliderInstance->GetUnequipAnimMontage());
			//	AnimInstance->SetWeaponType(EWeaponType::None);
			//}

			GliderInstance->Destroy();
			GliderInstance = nullptr;
		}

		// 글라이더 미장착 움직임 적용
		GetCharacterMovement()->GravityScale = 1.75f;
		GetCharacterMovement()->AirControl = 0.35f;

		// 인풋 초기화
		InputDirectionVector = FVector::ZeroVector;
		RightInputValue = 0.0f;
		ForwardInputValue = 0.0f;

		AnimInstance->SetGliding(false);
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

