#include "Character/GPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/GInputConfig.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Item/GWeaponActor.h"
#include "Item/GGliderActor.h"
#include "Animation/GAnimInstance.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Component/GStatComponent.h"
#include "GPlayerCharacterSettings.h"
#include "Character/GMonster.h"
#include "Controller/GPlayerController.h"
#include "DSP/AudioDebuggingUtilities.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Game/GPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "WorldStatic/GLandMine.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

AGPlayerCharacter::AGPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	RotationInterpRate = 5.f;
	BaseTurnRate = 50.f;
	BaseLookUpRate = 50.f;

	CurrentViewMode = EViewMode::None;

	bIsRun = false;
	bIsGliding = false;

	MaxJumpCount = 3;
	CurJumpCount = 0;
	bIsFliping = false;
	GliderAirControl = 0.7f;

	bIsDashing = false;

	bCanMoveInAttacking = false;
	
	bIsBasicAttacking = false;
	bIsChargedAttacking = false;
	bIsAirAttacking = false;
	bIsRunAttacking = false;
	bIsCrouchAttacking = false;
	bIsSkillFirstAttacking = false;
	bIsSkillSecondAttacking = false;

	bIsAiming = false;
	
	bIsGuarding = false;
	bIsParrying = false;

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(45.f);

	// BodyMesh
	HairBodyMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairBodyMeshComponent"));
	HairBodyMeshComponent->SetupAttachment(GetMesh());

	UpperBodyMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UpperBodyMeshComponent"));
	UpperBodyMeshComponent->SetupAttachment(GetMesh());

	LowerBodyMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LowerBodyMeshComponent"));
	LowerBodyMeshComponent->SetupAttachment(GetMesh());

	HandBodyMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandBodyMeshComponent"));
	HandBodyMeshComponent->SetupAttachment(GetMesh());

	FootBodyMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FootBodyMeshComponent"));
	FootBodyMeshComponent->SetupAttachment(GetMesh());

	// ClothMesh
	HelmetMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMeshComponent"));
	HelmetMeshComponent->SetupAttachment(GetMesh());

	ShirtMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShirtMeshComponent"));
	ShirtMeshComponent->SetupAttachment(GetMesh());

	PantMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PantMeshComponent"));
	PantMeshComponent->SetupAttachment(GetMesh());

	BootMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BootMeshComponent"));
	BootMeshComponent->SetupAttachment(GetMesh());

	// Particle
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(GetRootComponent());
	ParticleSystemComponent->SetAutoActivate(false);

	// GameProjectSettings/GPlayerCharacterSettings 읽어오기
	const UGPlayerCharacterSettings* CDO = GetDefault<UGPlayerCharacterSettings>();
	if (0 < CDO->PlayerCharacterMeshMaterialPaths.Num())
	{
		for (FSoftObjectPath PlayerCharacterMeshPath : CDO->PlayerCharacterMeshMaterialPaths)
		{
			UE_LOG(LogTemp, Warning, TEXT("Path: %s"), *(PlayerCharacterMeshPath.ToString()));
		}
	}
}

UE_DISABLE_OPTIMIZATION

void AGPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ForwardInputValue);
	DOREPLIFETIME(ThisClass, RightInputValue);
	DOREPLIFETIME(ThisClass, InputDirectionVector);

	DOREPLIFETIME(ThisClass, bIsRun);
	DOREPLIFETIME(ThisClass, bIsGliding);
	DOREPLIFETIME(ThisClass, CurJumpCount);
	DOREPLIFETIME(ThisClass, bIsDashing);

	DOREPLIFETIME(ThisClass, bIsBasicAttacking);
	DOREPLIFETIME(ThisClass, bIsChargedAttacking);
	DOREPLIFETIME(ThisClass, bIsAirAttacking);
	DOREPLIFETIME(ThisClass, bIsRunAttacking);
	DOREPLIFETIME(ThisClass, bIsCrouchAttacking);
	DOREPLIFETIME(ThisClass, bIsSkillFirstAttacking);
	DOREPLIFETIME(ThisClass, bIsSkillSecondAttacking);
	
	DOREPLIFETIME(ThisClass, bIsAiming);
	
	DOREPLIFETIME(ThisClass, bIsGuarding);
	DOREPLIFETIME(ThisClass, bIsParrying);

	DOREPLIFETIME(ThisClass, WeaponInstance);
	DOREPLIFETIME(ThisClass, GliderInstance);
}

void AGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// if(HasAuthority() == true)
	// 	return;
	//
	// if (GetOwner() != UGameplayStatics::GetPlayerController(this, 0))
	// 	return;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController) == true)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer());
		if (IsValid(Subsystem) == true)
		{
			Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
		}
	}

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	//AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnMontageEnded);
	//AnimInstance->OnCheckHit.AddDynamic(this, &ThisClass::OnCheckHit);// AnimInstance Delegate가 아닌 직접 만든 노티파이 이용
	AnimInstance->OnCheckAttackInput.AddDynamic(this, &ThisClass::OnCheckAttackInput);

	if (IsLocallyControlled() == true)
	{
		ensureMsgf(PlayerCharacterInputConfig != nullptr, TEXT("Invalid InputConfig"));
		ensureMsgf(PlayerCharacterInputMappingContext != nullptr, TEXT("Invalid InputMappingContext"));
		ensureMsgf(DirectionCurve != nullptr, TEXT("Invalid DirectionCurve"));
		ensureMsgf(WeaponClass != nullptr, TEXT("Invalid WeaponClass"));
		ensureMsgf(GliderClass != nullptr, TEXT("Invalid GliderClass"));
		ensureMsgf(PlayerUnarmedCharacterAnimLayer != nullptr, TEXT("Invalid PlayerUnarmedCharacterAnimLayer"));
	}

	GetMesh()->LinkAnimClassLayers(PlayerUnarmedCharacterAnimLayer);

	HairBodyMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	UpperBodyMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	LowerBodyMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	HandBodyMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	FootBodyMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);

	HelmetMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	ShirtMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	PantMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	BootMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);

	// 상의를 입었다면,
	//UpperBodyMeshComponent->SetHiddenInGame(true);
	
	// 바지를 입었다면,
	//LowerBodyMeshComponent->SetHiddenInGame(true);
	// 바지를 벗었다면,
	//LowerBodyMeshComponent->SetHiddenInGame(false);

	ensureMsgf(IsValid(ParticleSystemComponent), TEXT("Invalid ParticleSystemComponent"));

	ensureMsgf(IsValid(LandMineClass), TEXT("Invalid LandMineClass"));

	// const UGPlayerCharacterSettings* CDO = GetDefault<UGPlayerCharacterSettings>();
	// int32 RandIndex = FMath::RandRange(0, CDO->PlayerCharacterMeshMaterialPaths.Num() - 1);
	// CurrentPlayerCharacterMeshMaterialPath = CDO->PlayerCharacterMeshMaterialPaths[RandIndex];
	// AssetStreamableHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
	// 		CurrentPlayerCharacterMeshMaterialPath,
	// 		FStreamableDelegate::CreateLambda([this]() -> void
	// 		{
	// 			AssetStreamableHandle->ReleaseHandle();
	// 			TSoftObjectPtr<UMaterialInstance> LoadedMaterialInstanceAsset(CurrentPlayerCharacterMeshMaterialPath);
	// 			if (LoadedMaterialInstanceAsset.IsValid() == true)
	// 			{
	// 				LowerBodyMeshComponent->SetMaterial(1, LoadedMaterialInstanceAsset.Get());
	// 			}
	// 		})
	// );

	SetViewMode(EViewMode::BackView_UnLock);
}

void AGPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Debug
	{
		if(HasAuthority() == true)
		{
			// TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
			// UKismetSystemLibrary::PrintString(
			// 	this, FString::Printf(TEXT("MovementDirection is %hhd"), AnimInstance->GetMovementDirection()));

			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f _ Server"), DeltaTime));
		}
		if (IsLocallyControlled() == true)
		{
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f _ OwningClient"), DeltaTime));
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f _ OwningClient"), GetWorld()->GetDeltaSeconds()));
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f _ OwningClient"), GetWorld()->DeltaTimeSeconds));
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f _ OwningClient"), GetWorld()->TimeSeconds));
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f _ OwningClient"), GetWorld()->RealTimeSeconds));
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s _ OwningClient"), *GetActorRotation().ToString()));

			//TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
			//if (IsValid(AnimInstance) == true)
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%hhu"), bIsGliding));
			// UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%d"), CurJumpCount)
			// 	, true, true, FLinearColor(0, 0.66, 1), 2
			// 	, FName(TEXT("agfd")));

			// TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
			// UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MovementDirection is %hhd"), AnimInstance->GetMovementDirection()));

			
			// FRotator CurrentRotation = GetActorRotation();
			// FRotator TargetRotation = GetActorRotation();
			// if (InputDirectionVector.IsNearlyZero() == false && GetVelocity().IsNearlyZero() == false)
			// {
			// 	TargetRotation = UKismetMathLibrary::MakeRotFromX(InputDirectionVector);
			// }
			//
			// FRotator DeltaRot = CurrentRotation - TargetRotation;
			// DeltaRot.Normalize();
			//
			// constexpr float Tolerance = 1.0f;
			// if (FMath::Abs(DeltaRot.Yaw) < Tolerance)
			// {
			// 	//UKismetSystemLibrary::PrintString(this, TEXT("Rotation finished"));
			// }
			// else
			// {
			// 	//UKismetSystemLibrary::PrintString(this, TEXT("Rotation in progress"));
			//
			// 	float DeltaYaw = TargetRotation.Yaw - CurrentRotation.Yaw;
			// 	DeltaYaw = FMath::UnwindDegrees(DeltaYaw);
			// 	
			// 	if (DeltaYaw > 0)
			// 	{
			// 		//UKismetSystemLibrary::PrintString(this, TEXT("Rotating Right"));
			// 	}
			// 	else if (DeltaYaw < 0)
			// 	{
			// 		//UKismetSystemLibrary::PrintString(this, TEXT("Rotating Left"));
			// 	}
			// 	else
			// 	{
			// 		//UKismetSystemLibrary::PrintString(this, TEXT("Not Rotating"));
			// 	}
			// }
		}
	}

	// OwningClient
	// 줌 선형보간
	if (IsLocallyControlled() == true)
	{
		if (FMath::Abs(SpringArmComponent->TargetArmLength - ExpectedSpringArmLength) > 1.0f)
		{
			SpringArmComponent->TargetArmLength = FMath::Lerp(SpringArmComponent->TargetArmLength,
			                                                  ExpectedSpringArmLength, 10.f * DeltaTime);
		}
	}

	// // OwningClient 선적용 > Server 후보고
	// // 시점(움직임)관련 부분
	// //if (HasAuthority() == true)
	// if (HasAuthority() == false && IsLocallyControlled() == true)
	// {
	// 	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	// 	if (IsValid(AnimInstance) == true)
	// 	{
	// 		if (AnimInstance->GetAnimMoveType() == EAnimMoveType::Lock || bIsGliding == true)
	// 		{
	// 			if (GetCharacterMovement()->GetCurrentAcceleration().Length() > 0.f)
	// 			{
	// 				FRotator AccelerationRotationFromX = UKismetMathLibrary::MakeRotFromX(
	// 					GetCharacterMovement()->GetCurrentAcceleration());
	// 				FRotator ControlRotation = GetControlRotation();
	// 				FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(
	// 					AccelerationRotationFromX, ControlRotation);
	// 				double ApplyYaw = ControlRotation.Yaw + DirectionCurve->GetFloatValue(DeltaRotator.Yaw);
	//
	// 				FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(),
	// 				                                                     UKismetMathLibrary::MakeRotator(
	// 					                                                     0.0f, 0.0f, ApplyYaw),
	// 				                                                     DeltaTime,
	// 				                                                     RotationInterpRate);
	// 				SetActorRotation(NewRotation);
	//
	// 				UpdateRotation_Server(NewRotation);
	// 			}
	// 		}
	// 		else if (AnimInstance->GetAnimMoveType() == EAnimMoveType::UnLock && bIsGliding == false)
	// 		{
	// 			if (InputDirectionVector.IsNearlyZero() == false && GetVelocity().IsNearlyZero() == false)
	// 			{
	// 				FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(InputDirectionVector);
	// 				FRotator NewRotation = UKismetMathLibrary::RInterpTo(
	// 					GetActorRotation(), TargetRotation, DeltaTime, RotationInterpRate);
	//
	// 				// FRotator NewRotation = FRotator(0.0f,
	// 				// 	UKismetMathLibrary::Lerp(GetActorRotation().Yaw,
	// 				// 		TargetRotation.Yaw, DeltaTime * RotationInterpRate)
	// 				// 		, 0.0f);
	//
	// 				// FRotator NewRotation = UKismetMathLibrary::RInterpTo(
	// 				// 	GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), RotationInterpRate);
	// 				
	// 				SetActorRotation(NewRotation);
	// 				
	// 				UpdateRotation_Server(NewRotation);
	// 				
	// 				//UpdateRotation_NetMulticast(NewRotation);
	// 			}
	// 		}
	// 	}
	// }
	//
	// OwningClient 선적용 > Server 후보고
	// 글라이딩 관련 부분
	// if (IsLocallyControlled() == true)
	// {
	// 	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	// 	if (IsValid(AnimInstance) == true)
	// 	{
	// 		// 글라이딩 조작에 따른 액터가 회전하는 부분
	// 		if (AnimInstance->IsFalling() == true && bIsGliding == true)
	// 		{
	// 			//SetViewMode(EViewMode::BackView_Lock);
	// 			//AnimInstance->SetAnimMoveType(EAnimMoveType::Lock);
	// 			
	// 			if (InputDirectionVector.IsNearlyZero() == false)
	// 			{
	// 				// 무브먼트 Input에 따른 글라이딩 상태 회전 적용
	// 				constexpr float MaxAngle = 45.0f;
	// 				constexpr float InterpRate = 3.0f;
	//
	// 				float TargetRoll = RightInputValue * MaxAngle;
	// 				float TargetPitch = -ForwardInputValue * MaxAngle;
	// 				// if (AnimInstance->GetAnimMoveType() == EAnimMoveType::UnLock
	// 				// 	&& bIsGliding == false)
	// 				// {
	// 				// 	//UKismetSystemLibrary::PrintString(this, TEXT("FUCK"));
	// 				// 	// BackGeneralView인 경우에는 Backward 움직임 키를 누르더라도
	// 				// 	// Forward와 동일하게 동작하기위한 부분
	// 				// 	if (ForwardInputValue < 0)
	// 				// 		TargetPitch = -(TargetPitch);
	// 				// }
	//
	// 				FRotator TargetRotation = FRotator(TargetPitch, GetActorRotation().Yaw, TargetRoll);
	// 				FRotator NewRotation = UKismetMathLibrary::RInterpTo(
	// 					GetActorRotation(), TargetRotation, DeltaTime, InterpRate);
	// 				SetActorRotation(NewRotation);
	//
	// 				UpdateRotation_Server(NewRotation);
	//
	// 				// (Deprecated)
	// 				// 회전된 상태에서 바닥에 부딪혀도 bIsFalling이 초기화 되지않기에
	// 				// 캡슐 컴포넌트의 회전은 고정시키는 방식
	// 				//FRotator OriginalRotation = GetCapsuleComponent()->GetComponentRotation();
	// 				//FRotator FixedRotation = FRotator(0.f, OriginalRotation.Yaw, 0.f);
	// 				//GetCapsuleComponent()->SetWorldRotation(FixedRotation);
	// 			}
	// 			else
	// 			{
	// 				// Movement Input이 없는 경우에 원 상태로 회전시키는 부분
	// 				constexpr float InterpRate = 3.0f;
	// 				FRotator TargetRotation = FRotator(0.0f, GetActorRotation().Yaw, 0.0f);
	// 				// FRotator NewRotation = UKismetMathLibrary::RInterpTo(
	// 				// 	GetActorRotation(), TargetRotation, DeltaTime, InterpRate);
	// 				FRotator NewRotation = UKismetMathLibrary::RInterpTo_Constant(
	// 					GetActorRotation(), TargetRotation, DeltaTime, InterpRate);
	// 				
	// 				SetActorRotation(NewRotation);
	//
	// 				UpdateRotation_Server(NewRotation);
	// 			}
	// 		}
	// 		
	// 		// 바닥과 닿았는지 충돌 체크하는 부분
	// 		if (AnimInstance->IsFalling() == true && CurJumpCount >= 1)
	// 		{
	// 			// 회전된 상태에서 바닥에 부딪혀도 bIsFalling이 초기화 되지않기에
	// 			// 충돌체크를 통해 물체(바닥)와의 충돌 여부를 확인하고
	// 			// 충돌한 경우에 캐릭 pitch, roll 로테이션 초기화 진행
	// 			// 해당 부분은 Tick이 아닌 타이머를 통해 확인하는 것이 좋음
	//
	// 			if (GetActorRotation().Pitch != 0.0f || GetActorRotation().Roll != 0.0f)
	// 			{
	// 				constexpr float Range = 120.f;
	//
	// 				FHitResult HitResult;
	// 				FCollisionQueryParams Params(NAME_None, false, this);
	// 				Params.AddIgnoredActor(this);
	//
	// 				bool bResult = GetWorld()->LineTraceSingleByChannel(
	// 					HitResult,
	// 					GetActorLocation(),
	// 					// GetActorLocation() + Range * -GetActorUpVector(),
	// 					GetActorLocation() + FVector(0, 0, -Range), // 월드 좌표계에서 -Z 방향
	// 					ECC_GameTraceChannel1,
	// 					Params
	// 				);
	//
	// 				if (bResult == true)
	// 				{
	// 					if (IsValid(HitResult.GetActor()) == true)
	// 					{
	// 						UKismetSystemLibrary::PrintString(
	// 							this, FString::Printf(
	// 								TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));
	// 					}
	//
	// 					FRotator TargetRotation = FRotator(0.0f, GetActorRotation().Yaw, 0.0f);
	// 					SetActorRotation(TargetRotation);
	//
	// 					UpdateRotation_Server(TargetRotation);
	// 				}
	// 			}
	// 		}
	// 	}
	//}
}


void AGPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
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
	case EViewMode::BackView_UnLock:
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;// false가 핵심
		bUseControllerRotationRoll = false;

		SpringArmComponent->TargetArmLength = 400.f;
		SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);

		SpringArmComponent->bUsePawnControlRotation = true;

		SpringArmComponent->bInheritPitch = true;
		SpringArmComponent->bInheritYaw = true;
		SpringArmComponent->bInheritRoll = false;

		SpringArmComponent->bDoCollisionTest = true;

		GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
		GetCharacterMovement()->bOrientRotationToMovement = true;// true가 핵심
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

	case EViewMode::BackView_Lock:
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;// Lock은 true가 맞지만, false로 하고 입력이 있을 때만, true로 
		bUseControllerRotationRoll = false;

		SpringArmComponent->TargetArmLength = 400.f;
		SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);

		SpringArmComponent->bUsePawnControlRotation = true;

		SpringArmComponent->bInheritPitch = true;
		SpringArmComponent->bInheritYaw = true;
		SpringArmComponent->bInheritRoll = false;

		SpringArmComponent->bDoCollisionTest = true;

		GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
		GetCharacterMovement()->bOrientRotationToMovement = false;// false가 핵심
		GetCharacterMovement()->bUseControllerDesiredRotation = false;// Lock은 true가 맞지만, false로 하고 입력이 있을 때만, true로 

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

void AGPlayerCharacter::SetMeshMaterial(const EPlayerTeam& InPlayerTeam)
{
	uint8 TeamIdx = 0u;
	switch (InPlayerTeam)
	{
	case EPlayerTeam::Black:
		TeamIdx = 0u;
		break;
	case EPlayerTeam::White:
		TeamIdx = 1u;
		break;
	default:
		break;
	}

	const UGPlayerCharacterSettings* CDO = GetDefault<UGPlayerCharacterSettings>();
	CurrentPlayerCharacterMeshMaterialPath = CDO->PlayerCharacterMeshMaterialPaths[TeamIdx];
	AssetStreamableHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CurrentPlayerCharacterMeshMaterialPath,
		FStreamableDelegate::CreateLambda([this]() -> void
		{
			AssetStreamableHandle->ReleaseHandle();
			TSoftObjectPtr<UMaterialInstance> LoadedMaterialInstanceAsset(CurrentPlayerCharacterMeshMaterialPath);
			if (LoadedMaterialInstanceAsset.IsValid() == true)
			{
				LowerBodyMeshComponent->SetMaterial(1, LoadedMaterialInstanceAsset.Get());
			}
		})
	);
}

void AGPlayerCharacter::OnCheckHit()
{
	if(IsLocallyControlled() == false || HasAuthority() == true)
		return;
	
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("CheckHit() has been called.")));

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + BasicAttackRange * GetActorForwardVector(),
		FQuat::Identity,
		ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(BasicAttackRadius),
		Params
	);
	
	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			if (::IsValid(HitResult.GetActor()))
			{
				UKismetSystemLibrary::PrintString(
					this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));

				ApplyDamageAndDrawLine_Server(HitResult, true);
			}
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Hit Actor Name: None"));
		
		FHitResult NoHitResult;
		ApplyDamageAndDrawLine_Server(NoHitResult, false);
	}
}

void AGPlayerCharacter::OnCheckAttackInput()
{
	if(IsLocallyControlled() == false || HasAuthority() == true)
		return;

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnCheckAttackInput is called")));
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> BasicAttackAnimMontage = CurrentLinkedAnimInstance->GetBasicAttackAnimMontage();
	ensureMsgf(IsValid(BasicAttackAnimMontage), TEXT("Invalid BasicAttackAnimMontage"));

	// 구버전 Charged Attack
	// if (CurrentComboCount == 1 && bIsAttackKeyHold == static_cast<uint8>(true))
	// {
	// 	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Charged Attack")));
	//
	// 	EndBasicAttackCombo(nullptr, true);// EndBasicAttack 강제 호출
	// 	
	// 	ChargedAttack_Owner();
	// 	
	// 	return;
	// }
	
	if (bIsAttackKeyPressed == static_cast<uint8>(true))
	{
		// if (InputDirectionVector.IsNearlyZero() == false)
		// {
		// 	FRotator InputRotation = InputDirectionVector.Rotation();
		// 	this->SetActorRotation(InputRotation);
		// 	UpdateRotation_Server(InputRotation);
		// }
		
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

		FName NextSectionName = *FString::Printf(TEXT("%s%d"), *AttackAnimMontageSectionName, CurrentComboCount);
		AnimInstance->Montage_JumpToSection(NextSectionName, BasicAttackAnimMontage);
	}

	OnCheckAttackInput_Server(bIsAttackKeyPressed, CurrentComboCount);

	bIsAttackKeyPressed = false;
}

void AGPlayerCharacter::OnCheckUpdateRotation()
{
	if(IsLocallyControlled() == false || HasAuthority() == true)
		return;
	
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnUpdateRotation() has been called")));
	
	if (InputDirectionVector.IsNearlyZero() == false)
	{
		//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnUpdateRotation() has been called on Second")));
		FRotator InputRotation = InputDirectionVector.Rotation();
		this->SetActorRotation(InputRotation);
		UpdateRotation_Server(InputRotation);
	}
}

void AGPlayerCharacter::OnCheckUpdateCanMove(bool InCanMove)
{
	if(IsLocallyControlled() == false || HasAuthority() == true)
		return;
	
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnCheckUpdateCanMove() has been called")));

	bCanMoveInAttacking = InCanMove;
}

void AGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent) == true)
	{
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Move, ETriggerEvent::Triggered, this,
		                                   &ThisClass::InputMove);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Move, ETriggerEvent::Completed, this,
		                                   &ThisClass::InputMoveEnd);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Look, ETriggerEvent::Triggered, this,
		                                   &ThisClass::InputLook);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Zoom, ETriggerEvent::Triggered, this,
		                                   &ThisClass::InputZoom);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Jump, ETriggerEvent::Started, this,
		                                   &ThisClass::InputJumpStart);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Jump, ETriggerEvent::Completed, this,
		                                   &ThisClass::InputJumpEnd);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Equip, ETriggerEvent::Started, this,
		                                   &ThisClass::InputEquip);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Equip2, ETriggerEvent::Started, this,
										   &ThisClass::InputEquip2);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->UnEquip, ETriggerEvent::Started, this,
		                                   &ThisClass::InputUnEquip);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Run, ETriggerEvent::Started, this,
		                                   &ThisClass::InputRunStart);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Run, ETriggerEvent::Completed, this,
		                                   &ThisClass::InputRunEnd);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->ChangeAnimMoveType, ETriggerEvent::Started, this,
		                                   &ThisClass::InputChangeAnimMoveType);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Crouch, ETriggerEvent::Started, this,
		                                   &ThisClass::InputCrouch);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Dash, ETriggerEvent::Started, this,
		                                   &ThisClass::InputDash);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->BasicAttack, ETriggerEvent::Started, this,
		                                   &ThisClass::InputAttack);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->BasicAttack, ETriggerEvent::Completed, this,
										   &ThisClass::InputAttackEnd);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->SkillFirst, ETriggerEvent::Started, this,
										   &ThisClass::InputSkillFirst);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->SkillSecond, ETriggerEvent::Started, this,
										   &ThisClass::InputSkillSecond);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->ESCMenu, ETriggerEvent::Started, this,
		                                   &ThisClass::InputESCMenu);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->LandMine, ETriggerEvent::Started, this,
		                                   &ThisClass::SpawnLandMine);
	}
}

TObjectPtr<UGAnimInstance> AGPlayerCharacter::GetLinkedAnimInstance()
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	UGAnimInstance* CurrentLinkedAnimInstance = nullptr;
	if (AnimInstance->GetWeaponType() == EWeaponType::None)
	{
		CurrentLinkedAnimInstance = Cast<UGAnimInstance>(
			GetMesh()->GetLinkedAnimLayerInstanceByClass(PlayerUnarmedCharacterAnimLayer));
	}
	else
	{
		ensureMsgf(IsValid(WeaponInstance), TEXT("Invalid WeaponInstance"));
		CurrentLinkedAnimInstance = Cast<UGAnimInstance>(
			GetMesh()->GetLinkedAnimLayerInstanceByClass(WeaponInstance->GetArmedCharacterAnimLayer()));
	}
	ensureMsgf(IsValid(CurrentLinkedAnimInstance), TEXT("Invalid CurrentLinkedAnimInstance"));

	if(CurrentLinkedAnimInstance == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("CurrentLinkedAnimInstance is NULLPTR"));
	}
	
	CurrentLinkedAnimInstance->InitializeMainAnimInstance(AnimInstance);

	return CurrentLinkedAnimInstance;
}

void AGPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	
	
	// 서버 공통
	if(HasAuthority() == true)
	{
		CurJumpCount = 0;
	}

	// Owner 공통
	if(IsLocallyControlled() == true)
	{
		TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

		// 지금 대쉬하고도 여기 들어오고 있음
		// 대쉬는 UnLock으로 전환할 필요 없음
		// 해당 부분 수정 필요
		// 그리고 다시 RootMotion 시도 해보기

		if(bIsGliding == true || GetCharacterMovement()->IsFalling() == true)
		{
			GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
			
			// UKismetSystemLibrary::PrintString(this, TEXT("Landed is called"));
			//
			// // 착륙하는 경우, UnLock으로 전환
			// AnimInstance->SetAnimMoveType(EAnimMoveType::UnLock);
			// SetViewMode(EViewMode::BackView_UnLock);
			// UpdateAnimMoveType_Server(AnimInstance->GetAnimMoveType());
		}
	}

	// 특정 조건
	// bIsGlding
	if(bIsGliding == true)
	{
		if(IsLocallyControlled() == true)
		{
			// 글라이더 미장착 움직임 적용
			GetCharacterMovement()->GravityScale = 1.75f;
			GetCharacterMovement()->AirControl = 0.35f;
		}
		if(HasAuthority() == true)
		{
			UKismetSystemLibrary::PrintString(this, TEXT("Landed is called in Server"));
			// 착륙했을 경우에
			// 글라이더 제거 및 글라이더 미착용 관련 변수 설정
			DestroyGliderInstance_Server();
		}
	}

	// bIsAirAttacking
	if(bIsAirAttacking == true)
	{
		if(IsLocallyControlled() == true)
			LastSectionAirAttack_Owner();
	}
}

void AGPlayerCharacter::InputChangeAnimMoveType(const FInputActionValue& InValue)
{
	// TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	// ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	//
	// // 공중에서는 시점 전환 키 불가능
	// if(GetCharacterMovement()->IsFalling() == true || bIsGliding == true)
	// 	return;
	//
	// switch (AnimInstance->GetAnimMoveType())
	// {
	// case EAnimMoveType::Lock:
	// 	AnimInstance->SetAnimMoveType(EAnimMoveType::UnLock);
	// 	SetViewMode(EViewMode::BackView_UnLock);
	// 	break;
	//
	// case EAnimMoveType::UnLock:
	// 	AnimInstance->SetAnimMoveType(EAnimMoveType::Lock);
	// 	SetViewMode(EViewMode::BackView_Lock);
	// 	break;
	//
	// default:
	// 	break;
	// }
	//
	// UpdateAnimMoveType_Server(AnimInstance->GetAnimMoveType());
}

void AGPlayerCharacter::InputMove(const FInputActionValue& InValue)
{
	if (StatComponent->GetCurrentHP() <= KINDA_SMALL_NUMBER)
		return;

	// if (GetCharacterMovement()->GetGroundMovementMode() == MOVE_None)
	// 	return;

	// 여기서 예외처리하면, InputValue와 InputDirectionVector가 0임
	// if(bIsDashing == true)// Root Motion 이용중
	// 	return;

	// if(bIsBasicAttacking == true)// Root Motion 이용중
	// 	return;

	// InputValue와 InputDirectionVector을 전혀 이용할 가능성이 없으면
	// 여기서 예외처리 가능하긴 함
	if(bIsAirAttacking == true)
		return;
	
	if (IsValid(GetController()) == true)
	{
		FVector2D MovementVector = InValue.Get<FVector2D>();
		ForwardInputValue = MovementVector.X;
		RightInputValue = MovementVector.Y;
	
		switch (CurrentViewMode)
		{
		case EViewMode::BackView_UnLock:
			{
				const FRotator ControlRotation = GetController()->GetControlRotation();
				const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);
	
				const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
				const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);
				
				UpdateInputValue_Server(ForwardInputValue, RightInputValue);
	
				InputDirectionVector = FVector::ZeroVector;
				InputDirectionVector += ForwardVector * ForwardInputValue;
				InputDirectionVector += RightVector * RightInputValue;
				InputDirectionVector.Normalize();
	
				UpdateInputDirectionVector_Server(InputDirectionVector);

				// 움직임 제한하는 경우
				if(bIsDashing == true)// RootMotion 이용중
					break;

				if(bIsRunAttacking == true)// RootMotion 이용중
					break;
				
				if(bIsBasicAttacking == true)// RootMotion 이용중
					break;

				if(bIsChargedAttacking == true)
				{
					TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
					if(IsValid(AnimInstance) == true)
					{
						if(AnimInstance->GetWeaponType() == EWeaponType::None)
						{
							// 움직임 불가
							break;
						}
						else if(AnimInstance->GetWeaponType() == EWeaponType::GreatSword)
						{
							// 계속 움직임 실행
						}
						else if(AnimInstance->GetWeaponType() == EWeaponType::Bow)
						{
							// 계속 움직임 실행
						}
					}
				}

				if(bIsSkillFirstAttacking == true)
				{
					TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
					if(IsValid(AnimInstance) == true)
					{
						if(AnimInstance->GetWeaponType() == EWeaponType::None)
						{
							// 움직임 불가
							break;
						}
						else if(AnimInstance->GetWeaponType() == EWeaponType::GreatSword)
						{
							// 재생 중 회전 가능한 경우에만 움직임 가능
							if(bCanMoveInAttacking == false)
								break;
						}
						else if(AnimInstance->GetWeaponType() == EWeaponType::Bow)
						{
							// 재생 중 회전 가능한 경우에만 움직임 가능
							if(bCanMoveInAttacking == false)
								break;
						}
					}
				}
				
				if(bIsSkillSecondAttacking == true)
				{
					TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
					if(IsValid(AnimInstance) == true)
					{
						if(AnimInstance->GetWeaponType() == EWeaponType::None)
						{
							// 움직임 불가
							break;
						}
						else if(AnimInstance->GetWeaponType() == EWeaponType::GreatSword)
						{
							// 재생 중 회전 가능한 경우에만 움직임 가능
							if(bCanMoveInAttacking == false)
								break;
						}
						else if(AnimInstance->GetWeaponType() == EWeaponType::Bow)
						{
							// 재생 중 회전 가능한 경우에만 움직임 가능
							if(bCanMoveInAttacking == false)
								break;
						}
					}
				}
				
				AddMovementInput(ForwardVector, MovementVector.X);
				AddMovementInput(RightVector, MovementVector.Y);
				break;
			}

		case EViewMode::BackView_Lock:
			{
				// 공중에서는 Lock을 풀지 않도록 구현
				if(GetCharacterMovement()->IsFalling() == false && bIsGliding == false)
				{
					// 특정 각도 이상 인풋 회전하면, UnLock으로 전환
					if (FMath::Abs(FMath::FindDeltaAngleDegrees(GetActorRotation().Yaw, GetControlRotation().Yaw)) > ViewModeLockDegreeLimit)
					{
						UKismetSystemLibrary::PrintString(this, TEXT("ViewMode is changed to UnLock"));
						
						TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
						ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
					
						AnimInstance->SetAnimMoveType(EAnimMoveType::UnLock);
						SetViewMode(EViewMode::BackView_UnLock);
						UpdateAnimMoveType_Server(AnimInstance->GetAnimMoveType());
					
						break;
					}
				}
				
				//bUseControllerRotationYaw = true;// Lock은 true가 맞지만, false로 하고 입력이 있을 때만, true로 
				//GetCharacterMovement()->bUseControllerDesiredRotation = true;// Lock은 true가 맞지만, false로 하고 입력이 있을 때만, true로 
				
				const FRotator ControlRotation = GetController()->GetControlRotation();
				const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);
	
				const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
				const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

				AddMovementInput(ForwardVector, MovementVector.X);
				AddMovementInput(RightVector, MovementVector.Y);
	
				UpdateInputValue_Server(ForwardInputValue, RightInputValue);
	
				InputDirectionVector = FVector::ZeroVector;
				InputDirectionVector += ForwardVector * ForwardInputValue;
				InputDirectionVector += RightVector * RightInputValue;
				InputDirectionVector.Normalize();
	
				UpdateInputDirectionVector_Server(InputDirectionVector);
	
				break;
			}
	
		case EViewMode::None:
	
		case EViewMode::End:
	
		default:
			{
				// AddMovementInput(GetActorForwardVector(), MovementVector.X);
				// AddMovementInput(GetActorRightVector(), MovementVector.Y);
				//
				// InputDirectionVector = FVector::ZeroVector;
				// InputDirectionVector += GetActorForwardVector() * ForwardInputValue;
				// InputDirectionVector += GetActorRightVector() * RightInputValue;
				// InputDirectionVector.Normalize();
				//
				// UpdateInputValue_Server(ForwardInputValue, RightInputValue);
				// UpdateInputDirectionVector_Server(InputDirectionVector);
				// break;
			}
		}
	}
}

void AGPlayerCharacter::InputMoveEnd(const FInputActionValue& InValue)
{
	if (StatComponent->GetCurrentHP() <= KINDA_SMALL_NUMBER)
		return;

	if(bIsAirAttacking == true)
		return;

	// // 공중에서는 InputMove가 없더라도 기존 ControllerRotationYaw 계속 사용
	// if(GetCharacterMovement()->IsFalling() == false && bIsGliding == false)
	// {
	// 	bUseControllerRotationYaw = false;// Lock은 true가 맞지만, false로 하고 입력이 없을 때는, false로
	// 	GetCharacterMovement()->bUseControllerDesiredRotation = false;// Lock은 true가 맞지만, false로 하고 입력이 없을 때는, false로 
	// }
		
	// 인풋 초기화
	RightInputValue = 0.0f;
	ForwardInputValue = 0.0f;
	InputDirectionVector = FVector::ZeroVector;
	
	UpdateInputValue_Server(ForwardInputValue, RightInputValue);
	UpdateInputDirectionVector_Server(InputDirectionVector);
}

void AGPlayerCharacter::InputLook(const FInputActionValue& InValue)
{
	if (IsValid(GetController()) == true)
	{
		FVector2D LookVector = InValue.Get<FVector2D>();
	
		switch (CurrentViewMode)
		{
		case EViewMode::BackView_UnLock:
			//AddControllerYawInput(LookVector.X * BaseTurnRate * GetWorld()->GetDeltaSeconds());
			//AddControllerPitchInput(LookVector.Y * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
			AddControllerYawInput(LookVector.X);
			AddControllerPitchInput(LookVector.Y);
			break;

		case EViewMode::BackView_Lock:
			//AddControllerYawInput(LookVector.X * BaseTurnRate * GetWorld()->GetDeltaSeconds());
			//AddControllerPitchInput(LookVector.Y * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
			AddControllerYawInput(LookVector.X);
			AddControllerPitchInput(LookVector.Y);
			break;
	
		case EViewMode::None:
	
		case EViewMode::End:
	
		default:
			break;
		}
	}
}

void AGPlayerCharacter::InputZoom(const FInputActionValue& InValue)
{
	FVector2D ZoomVector = InValue.Get<FVector2D>();
	float InputValue = ZoomVector.X;
	
	if (FMath::Abs(InputValue) > 0.1f)
	{
		if (InputValue < 0.0f)
		{
			ExpectedSpringArmLength = FMath::Clamp<float>(ExpectedSpringArmLength + 30.0f, 250.0f, 1000.0f);
		}
		else
		{
			ExpectedSpringArmLength = FMath::Clamp<float>(ExpectedSpringArmLength - 30.0f, 250.0f, 1000.0f);
		}
	}
}

void AGPlayerCharacter::InputJumpStart(const FInputActionValue& InValue)
{
	if (StatComponent->GetCurrentHP() <= KINDA_SMALL_NUMBER)
		return;

	if(bIsDashing == true)
		return;
	
	if(bIsAirAttacking == true)
		return;

	// 사실상 없어도 되는 부분
	if (GetOwner() != UGameplayStatics::GetPlayerController(this, 0))
		return;

	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("JumpStart() has been called in OwningClient.")));
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("CurJumpCount is %d"), CurJumpCount));

	// 임시로 작성한 부분
	// Gliding 상태중에는 회전 천천히 하게
	GetCharacterMovement()->RotationRate = FRotator(0.f, 100.f, 0.f);
	
	JumpStart_Owner();

	JumpStart_Server();
}

void AGPlayerCharacter::InputJumpEnd(const FInputActionValue& InValue)
{
	if (StatComponent->GetCurrentHP() <= KINDA_SMALL_NUMBER)
		return;

	if (GetOwner() != UGameplayStatics::GetPlayerController(this, 0))
		return;

	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("JumpEnd() has been called in OwningClient.")));

	JumpEnd_Owner();
	
	JumpEnd_Server();
}

void AGPlayerCharacter::InputEquip(const FInputActionValue& InValue)
{
	if (StatComponent->GetCurrentHP() <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	if (AnimInstance->GetWeaponType() == EWeaponType::GreatSword)
	{
		return;
	}

	// 선 애니메이션 재생
	// WeaponInstance는 아직 없기에, WeaponClass DefaultObject 활용
	if (IsValid((WeaponClass)) == true)
	{
		if (TObjectPtr<AGWeaponActor> DefaultWeapon = WeaponClass->GetDefaultObject<AGWeaponActor>())
		{
			//AnimInstance->PlayAnimMontage(DefaultWeapon->GetEquipAnimMontage());
		}
	}

	if (IsValid((WeaponInstance)) == true)// 다른 무기가 이미 있으면, 제거
	{
		DestroyWeaponInstance_Server();
	}

	SpawnWeaponInstance_Server(1);

	AnimInstance->SetWeaponType(EWeaponType::GreatSword);
}

void AGPlayerCharacter::InputEquip2(const FInputActionValue& InValue)
{
	if (StatComponent->GetCurrentHP() <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	if (AnimInstance->GetWeaponType() == EWeaponType::Bow)
	{
		return;
	}

	// 선 애니메이션 재생
	// WeaponInstance는 아직 없기에, WeaponClass DefaultObject 활용
	if (IsValid((WeaponClass2)) == true)
	{
		if (TObjectPtr<AGWeaponActor> DefaultWeapon = WeaponClass2->GetDefaultObject<AGWeaponActor>())
		{
			//AnimInstance->PlayAnimMontage(DefaultWeapon->GetEquipAnimMontage());
		}
	}

	if (IsValid((WeaponInstance)) == true)// 다른 무기가 이미 있으면, 제거
	{
		DestroyWeaponInstance_Server();
	}
	
	SpawnWeaponInstance_Server(2);

	AnimInstance->SetWeaponType(EWeaponType::Bow);
}

void AGPlayerCharacter::InputUnEquip(const FInputActionValue& InValue)
{
	if (StatComponent->GetCurrentHP() <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	if (AnimInstance->GetWeaponType() == EWeaponType::None)
	{
		return;
	}

	// 선 애니메이션 재생
	if (IsValid((WeaponClass)) == true)
	{
		if (TObjectPtr<AGWeaponActor> DefaultWeapon = WeaponClass->GetDefaultObject<AGWeaponActor>())
		{
			//AnimInstance->PlayAnimMontage(DefaultWeapon->GetUnequipAnimMontage());
		}
	}

	DestroyWeaponInstance_Server();
	
	AnimInstance->SetWeaponType(EWeaponType::None);
}

void AGPlayerCharacter::InputRunStart(const FInputActionValue& InValue)
{
	do
	{
		if (StatComponent->GetCurrentHP() <= KINDA_SMALL_NUMBER)
			break;

		TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance) == false)
			break;

		if (AnimInstance->GetLocomotionState() != ELocomotionState::Walk)
			break;

		// MaxWalkSpeed는 동기화 되지 않도록 해놓은 상태
		bIsRun = true;
		SetWalkSpeed(600.f);

		RunStart_Server();
	}
	while (false);
}

void AGPlayerCharacter::InputRunEnd(const FInputActionValue& InValue)
{
	if (StatComponent->GetCurrentHP() <= KINDA_SMALL_NUMBER)
		return;

	if (bIsRun == false)
		return;

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) != true)
		return;

	// MaxWalkSpeed는 동기화 되지 않도록 해놓은 상태기에 클라이언트에서 실행
	bIsRun = false;
	SetWalkSpeed(300.f);

	RunEnd_Server();
}



void AGPlayerCharacter::InputCrouch(const FInputActionValue& InValue)
{
	// CrouchChange_Owner();
	//
	// CrouchChange_Server();
}

void AGPlayerCharacter::InputDash(const FInputActionValue& InValue)
{
	if (InputDirectionVector.IsNearlyZero() == false)
	{
		FRotator InputRotation = InputDirectionVector.Rotation();
		this->SetActorRotation(InputRotation);
		UpdateRotation_Server(InputRotation);
	}
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	if (AnimInstance->IsFalling())
		return;

	if (bIsDashing == true)
	 	return;
	
	Dash_Owner();
}

void AGPlayerCharacter::InputAttack(const FInputActionValue& InValue)
{
	if (StatComponent->GetCurrentHP() <= KINDA_SMALL_NUMBER)
		return;
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// AirAttack
	if (GetCharacterMovement()->IsFalling() == true)
	{
		if(bIsGliding == true)
			return;

		if(bIsAirAttacking == true)
			return;
		
		// 바닥과 너무 가까우면 return? (고려중)
		
		AirAttack_Owner();
		
		return;
	}

	// 일단 RunAttack 미사용 예정
	// RunAttack
	if(bIsRun == true)
	{
		if(bIsRunAttacking == true)
			return;

		//InputRunEnd(FVector::OneVector);
		
		RunAttack_Owner();
		
		return;
	}

	// CrouchAttack
	if (AnimInstance->IsCrouching() == true)
	{
		if(bIsCrouchAttacking == true)
			return;

		// CrouchAttack 미사용 예정
		// CrouchAttack_Owner();
		// 	return;

		// Crouch 상태에서 InputAttack 들어오는 경우
		// UnCrouch로 전환
		CrouchChange_Owner();
		CrouchChange_Server();
	}

	// ParryAttack
	// if(false)
	// {
	// 	// 여기서 호출하는 것이 아닌
	// 	// Monster가 공격할 때, CollisionTrace를 통해 플레이어가 Parry 상태(AnimNotifyState)인지 체크하고
	// 	// Parry 상태라면, 플레이어는 OnParryAttack 몬스터는 OnParryAttacked 실행
	//
	// 	ParryAttack_Owner();// 여기서 조건 확인 및 애님 몽타주 실행
	// 	ParryAttack_Server();// NetMulticast를 위한 통로
	// 	ParryAttack_NetMulticast();// Server와 Owner 제외하고 애님 몽타주 실행
	//
	// 	return;
	// }

	// Finisher Attack
	// if(false)
	// {
	// 	// 여기서 호출하는 것이 아닌
	// 	// Monster의 체력이 10퍼 이하라면,
	// 	// Monster가 FinisherAttack 위젯을 띄우고 bCanbeFinisherAttacked 상태 true로 설정
	//	// 위와같은 몬스터 상태에서 플레이어가 FinisherAttack Key를 누르면
	// 	// CollisionTrace를 통해 몬스터가 bCanbeFinisherAttacked 상태인지 체크하고
	// 	// 맞다면, 플레이어는 OnFinisherAttack 몬스터는 OnFinisherAttacked 실행
	//
	// 	FinisherAttack_Owner();// 여기서 조건 확인 및 애님 몽타주 실행
	// 	FinisherAttack_Server();// NetMulticast를 위한 통로
	// 	FinisherAttack_NetMulticast();// Server와 Owner 제외하고 애님 몽타주 실행
	//
	// 	return;
	// }

	// BasicAttack OR ChargedAttack
	if(CurrentComboCount == 0)// || CurrentComboCount == 3)
	{
		if(CurrentComboCount == 0)
			BeginBasicAttackCombo();// 일단 BasicAttack 실행
		
		bIsCharging = true;
     	bChargedAttackDetermined = false;
     	ChargedAttackTime = 0.0f;
		
     	auto BasicORChargingAttacklambda = [this]()
     	{
     		if(bIsCharging == false)
     		{
     			bChargedAttackDetermined = false;
     			//BeginBasicAttackCombo();
     		}
     		else// ChargedAttack
     		// Charging 중이면, BasicAttack 멈추고 ChargedAttack 실행
     		{
     			bChargedAttackDetermined = true;
     			EndBasicAttackCombo(nullptr, true);
     			ChargedAttack_Owner();
     		}
     	};
		
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda(BasicORChargingAttacklambda);
		
		GetWorldTimerManager().SetTimer(AttackTimerHandle, TimerDelegate, ChargeThreshold, false);
	}
	else
	{
		// OnCheckAttackInput를 통해 두번째와 세번째 콤보 몽타주 섹션 실행
		ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
		bIsAttackKeyPressed = true;
		
		// if (CurrentComboCount == 2)
		// {
		// 	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		// }
	}
}

void AGPlayerCharacter::InputAttackEnd(const FInputActionValue& InValue)
{
	bIsCharging = false;

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	if(AnimInstance->GetWeaponType() == EWeaponType::Bow)
	{
		bIsAiming = false;
	}
	
	//EndChargedAttack_Owner();
}

void AGPlayerCharacter::InputSkillFirst(const FInputActionValue& InValue)
{
	// 조건
	// ex. 특정 공격중에는 사용불가
	// ex. 반면에 특정 공격하다가 끊어서 사용 가능한 경우도 있을 수 있음
	// ex. Dash, Falling, Crouch 등 여러 상황에 맞게

	if(bIsSkillFirstAttacking == true)
		return;

	if (InputDirectionVector.IsNearlyZero() == false)
	{
		FRotator InputRotation = InputDirectionVector.Rotation();
		this->SetActorRotation(InputRotation);
		UpdateRotation_Server(InputRotation);
	}
	
	SkillFirst_Owner();
	
}

void AGPlayerCharacter::InputSkillSecond(const FInputActionValue& InValue)
{
	// 조건
	
	if(bIsSkillSecondAttacking == true)
		return;

	if (InputDirectionVector.IsNearlyZero() == false)
	{
		FRotator InputRotation = InputDirectionVector.Rotation();
		this->SetActorRotation(InputRotation);
		UpdateRotation_Server(InputRotation);
	}
	
	SkillSecond_Owner();
}

void AGPlayerCharacter::InputESCMenu(const FInputActionValue& InValue)
{
	AGPlayerController* PlayerController = GetController<AGPlayerController>();
	if (::IsValid(PlayerController) == true)
	{
		PlayerController->ToggleInGameESCMenu();
	}
}


void AGPlayerCharacter::UpdateInputValue_Server_Implementation(const float& InForwardInputValue,
                                                               const float& InRightInputValue)
{
	ForwardInputValue = InForwardInputValue;
	RightInputValue = InRightInputValue;
}

void AGPlayerCharacter::UpdateInputDirectionVector_Server_Implementation(const FVector& NewInputDirectionVector)
{
	InputDirectionVector = NewInputDirectionVector;
}

void AGPlayerCharacter::UpdateRotation_Server_Implementation(FRotator NewRotation)
{
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("UpdateRotation_Server")));
	
	SetActorRotation(NewRotation);
	UpdateRotation_NetMulticast(NewRotation);
}

void AGPlayerCharacter::UpdateRotation_NetMulticast_Implementation(FRotator NewRotation)
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;

	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("UpdateRotation_NetMulticast")));
	
	SetActorRotation(NewRotation);
}

void AGPlayerCharacter::UpdateAnimMoveType_Server_Implementation(EAnimMoveType NewAnimMoveType)
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	AnimInstance->SetAnimMoveType(NewAnimMoveType);

	UpdateAnimMoveType_NetMulticast(NewAnimMoveType);
}

void AGPlayerCharacter::UpdateAnimMoveType_NetMulticast_Implementation(EAnimMoveType NewAnimMoveType)
{
	if (HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	AnimInstance->SetAnimMoveType(NewAnimMoveType);
	
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AnimMoveType: %d in OtherClient"), AnimInstance->GetAnimMoveType()));
	
}

void AGPlayerCharacter::JumpStart_Owner()
{
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("CurJumpCount: %d in OwningClient"), CurJumpCount));

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// // 점프 시작하면, Lock으로 전환
	// AnimInstance->SetAnimMoveType(EAnimMoveType::Lock);
	// SetViewMode(EViewMode::BackView_Lock);
	// UpdateAnimMoveType_Server(AnimInstance->GetAnimMoveType());
	
	// LaunchCharacter
	float JumpZVelocityMultiplier = 1.0f;

		if (CurJumpCount == 0) // 1단 점프 예측
		{

		}
		else if (CurJumpCount == 1) // 2단 점프 예측
		{
			// LinkedAnimInstance의 AnimMontage 가져오기
			TObjectPtr<UGAnimInstance> CurrentLinkedAnimInstance = GetLinkedAnimInstance();
			TObjectPtr<UAnimMontage> JumpFlipAnimMontage = CurrentLinkedAnimInstance->GetJumpFlipAnimMontage();
			ensureMsgf(IsValid(JumpFlipAnimMontage), TEXT("Invalid JumpFlipAnimMontage"));

			AnimInstance->PlayAnimMontage(JumpFlipAnimMontage);

			if (OnJumpFlipMontageEndedDelegate.IsBound() == false)
			{
				OnJumpFlipMontageEndedDelegate.BindUObject(this, &ThisClass::EndJumpFlip);
				AnimInstance->Montage_SetEndDelegate(OnJumpFlipMontageEndedDelegate, JumpFlipAnimMontage);
			}
			
		}
		else if (CurJumpCount == 2) // 3단 점프 예측
		{
			//GetCharacterMovement()->Velocity.Z = 0.0f;
			GetCharacterMovement()->GravityScale = 0.05f;
			GetCharacterMovement()->AirControl = GliderAirControl;
			
			// LinkedAnimInstance의 AnimMontage 가져오기
			TObjectPtr<UGAnimInstance> CurrentLinkedAnimInstance = GetLinkedAnimInstance();
			TObjectPtr<UAnimMontage> GlidingStartAnimMontage = CurrentLinkedAnimInstance->GetGlidingStartAnimMontage();
			ensureMsgf(IsValid(GlidingStartAnimMontage), TEXT("Invalid GlidingStartAnimMontage"));
		
			AnimInstance->PlayAnimMontage(GlidingStartAnimMontage);
		}
		else if (CurJumpCount >= 3)// 하강 중 글라이더 해제한 후 다시 착용하는 경우 예측
		{
			// 글라이더 해제한 후 다시 착용하는 경우 움직임 설정
			GetCharacterMovement()->Velocity.Z = 0.0f;
			GetCharacterMovement()->GravityScale = 0.05f;
			GetCharacterMovement()->AirControl = GliderAirControl;
		}
}

void AGPlayerCharacter::JumpStart_Server_Implementation()
{
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("JumpStart() has been called in Server.")));
	
	++CurJumpCount;
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("CurJumpCount: %d in Server First"), CurJumpCount));

	// LaunchCharacter
		float JumpZVelocityMultiplier = 1.0f;

		if (CurJumpCount == 1) // 1단 점프
		{
			LaunchCharacter(
				FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity * JumpZVelocityMultiplier), false,
				true);
		}
		else if (CurJumpCount == 2) // 2단 점프
		{
			JumpZVelocityMultiplier = 1.5f; // 중력으로 인해 더 힘을 줘야 함
			LaunchCharacter(
				FVector(0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity * JumpZVelocityMultiplier), false,
				true);
		}
		else if (CurJumpCount == 3) // 3단 점프
		{
			//GetCharacterMovement()->Velocity.Z = 0.0f;
			GetCharacterMovement()->GravityScale = 0.05f;
			GetCharacterMovement()->AirControl = GliderAirControl;

			// 입력이 없는 경우 액터 포워드 방향으로
			FVector DiagonalVector = FVector::ZeroVector;
			if(InputDirectionVector.IsNearlyZero() == true)
			{
				DiagonalVector = GetActorForwardVector();
			}
			// 입력이 있는 경우 입력 방향으로
			else
			{
				FRotator Rotator = InputDirectionVector.Rotation();
				Rotator.Pitch += 0.0f; // 각도 위로 Pitch 회전
				DiagonalVector = FRotationMatrix(Rotator).GetUnitAxis(EAxis::X);
			}
			LaunchCharacter(DiagonalVector * 1500.0f, false, true);
			
			DestroyWeaponInstance_Server();

			SpawnGliderInstance_Server(true);
		}
		else if (CurJumpCount >= 4)// 하강 중 글라이더 해제한 후 다시 착용하는 경우
		{
			// 글라이더 해제한 후 다시 착용하는 경우 움직임 설정
			GetCharacterMovement()->Velocity.Z = 0.0f;
			GetCharacterMovement()->GravityScale = 0.05f;
			GetCharacterMovement()->AirControl = GliderAirControl;
			
			DestroyWeaponInstance_Server();

			SpawnGliderInstance_Server(false);
		}

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("CurJumpCount: %d in Server Second"), CurJumpCount));
	JumpStart_NetMulticast(CurJumpCount);
}

void AGPlayerCharacter::JumpStart_NetMulticast_Implementation(int32 InCurJumpCount)
{
	if (HasAuthority() == true || IsLocallyControlled() == true)
		return;

	CurJumpCount = InCurJumpCount;
	
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("JumpStart() has been called in NetMulticast.")));
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("CurJumpCount: %d in OtherClient"), CurJumpCount));
	
	if (TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		if (CurJumpCount == 2)// 2단 점프
		{
			// LinkedAnimInstance의 AnimMontage 가져오기
			TObjectPtr<UGAnimInstance> CurrentLinkedAnimInstance = GetLinkedAnimInstance();
			TObjectPtr<UAnimMontage> JumpFlipAnimMontage = CurrentLinkedAnimInstance->GetJumpFlipAnimMontage();

			AnimInstance->PlayAnimMontage(JumpFlipAnimMontage);
		}
	}
}

void AGPlayerCharacter::EndJumpFlip(UAnimMontage* InMontage, bool bInterrupted)
{
	if (OnJumpFlipMontageEndedDelegate.IsBound() == true)
	{
		OnJumpFlipMontageEndedDelegate.Unbind();
	}
	
	bIsFliping = false;
	
	EndJumpFlip_Server();
}

void AGPlayerCharacter::EndJumpFlip_Server_Implementation()
{
	bIsFliping = false;

	EndDJumpFlip_NetMulticast();
}

void AGPlayerCharacter::EndDJumpFlip_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
}

void AGPlayerCharacter::JumpEnd_Owner()// Deprecated
{
	// Hop 기능 (Deprecated)
	//if (GetCharacterMovement()->Velocity.Z > 0.0f)
	//{
	//	GetCharacterMovement()->Velocity = 
	//		UKismetMathLibrary::MakeVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0.0f);
	//}
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	// 글라이딩 중이라면, 글라이더 장착 해제
	if (bIsGliding == true && AnimInstance->IsFalling() == true)
	{
		// if (IsValid(GliderInstance) == true)
		// {
		// 	// 글라이더 장착 애님 몽타주 미사용
		// 	if (IsValid(GliderInstance->GetUnequipAnimMontage()))
		// 	{
		// 		AnimInstance->PlayAnimMontage(GliderInstance->GetUnequipAnimMontage());
		// 		AnimInstance->SetWeaponType(EWeaponType::None);
		// 	}
		// }
		
		// 글라이더 미장착 움직임 적용
		GetCharacterMovement()->GravityScale = 1.75f;
		GetCharacterMovement()->AirControl = 0.35f;

		// 인풋 초기화
		RightInputValue = 0.0f;
		ForwardInputValue = 0.0f;
		InputDirectionVector = FVector::ZeroVector;

		UpdateInputValue_Server(ForwardInputValue, RightInputValue);
		UpdateInputDirectionVector_Server(InputDirectionVector);
	}
}

void AGPlayerCharacter::JumpEnd_Server_Implementation()
{
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("JumpEnd() has been called in Server.")));

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// 글라이딩 중이라면, 글라이더 장착 해제
	if (bIsGliding == true && AnimInstance->IsFalling() == true)
	{
		DestroyGliderInstance_Server();
	}

	//JumpEnd_NetMulticast();
}

void AGPlayerCharacter::JumpEnd_NetMulticast_Implementation()// Deprecated
{
	// if (HasAuthority() == false && IsLocallyControlled() == false)
	// {
	// 	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("JumpEnd() has been called in NetMulticast.")));
	// }
}

void AGPlayerCharacter::RunStart_Server_Implementation()
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("RunStart() has been called in Server.")));

	bIsRun = true;
	SetWalkSpeed(600.f);

	// Other로 WalkSpeed 동기화는 되지 않는 상태
	// bIsInputRun은 Replicated 키워드를 사용해서 동기화 됨
	//RunStart_NetMulticast();
}

void AGPlayerCharacter::RunEnd_Server_Implementation()
{
	bIsRun = false;
	SetWalkSpeed(300.f);
}

void AGPlayerCharacter::CrouchChange_Owner()
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	if (AnimInstance->IsFalling())
		return;

	if (GetCharacterMovement()->IsCrouching() == false)
	{
		ACharacter::Crouch();
	}
	else
	{
		ACharacter::UnCrouch();
	}
}

void AGPlayerCharacter::CrouchChange_Server_Implementation()
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	if (AnimInstance->IsFalling())
		return;

	if (GetCharacterMovement()->IsCrouching() == false)
	{
		ACharacter::Crouch();
	}
	else
	{
		ACharacter::UnCrouch();
	}
}

void AGPlayerCharacter::Dash_Owner()
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	
	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> DashAnimMontage = CurrentLinkedAnimInstance->GetDashAnimMontage(AnimInstance->GetMovementDirection());

	bIsDashing = true;
	
	AnimInstance->PlayAnimMontage(DashAnimMontage);
	
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MovementDirection is %hhd"), AnimInstance->GetMovementDirection()));
	
	if (OnDashMontageEndedDelegate.IsBound() == false)
	{
		OnDashMontageEndedDelegate.BindUObject(this, &ThisClass::EndDash);
		AnimInstance->Montage_SetEndDelegate(OnDashMontageEndedDelegate, DashAnimMontage);
	}
	
	Dash_Server(DashAnimMontage);
}

void AGPlayerCharacter::Dash_Server_Implementation(UAnimMontage* InMontage)
{
	UKismetSystemLibrary::PrintString(this, TEXT("Dash_Server is called"));
	
	// Launch
	FRotator CurrentRotation = InputDirectionVector.Rotation();
	if (InputDirectionVector.IsNearlyZero())
	{
		CurrentRotation = GetActorForwardVector().Rotation();
	}
	FVector DashDirection = FRotationMatrix(CurrentRotation).GetUnitAxis(EAxis::X);
	//LaunchCharacter(DashDirection * 700.f, false, true);
	
	bIsDashing = true;

	Dash_NetMulticast(InMontage);
}

void AGPlayerCharacter::Dash_NetMulticast_Implementation(UAnimMontage* InMontage)
{
	if(IsLocallyControlled() == true)
		return;
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	// // LinkedAnimInstance의 AnimMontage 가져오기
	// UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	// TObjectPtr<UAnimMontage> DashAnimMontage = CurrentLinkedAnimInstance->GetDashAnimMontage();

	AnimInstance->PlayAnimMontage(InMontage);
}

void AGPlayerCharacter::EndDash(UAnimMontage* InMontage, bool bInterrupted)
{
	if (OnDashMontageEndedDelegate.IsBound() == true)
	{
		OnDashMontageEndedDelegate.Unbind();
	}
	
	bIsDashing = false;
	
	EndDash_Server();
}

void AGPlayerCharacter::EndDash_Server_Implementation()
{
	bIsDashing = false;

	EndDash_NetMulticast();
}

void AGPlayerCharacter::EndDash_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
}

void AGPlayerCharacter::SpawnLandMine(const FInputActionValue& InValue)
{
	SpawnLandMine_Server();
}

void AGPlayerCharacter::SpawnLandMine_Server_Implementation()
{
	ensureMsgf(IsValid(LandMineClass), TEXT("Invalid LandMineClass"));
	FVector SpawnedLocation = (GetActorLocation() + GetActorForwardVector() * 300.f) - FVector(0.f, 0.f, 90.f);
	AGLandMine* SpawnedLandMine = GetWorld()->SpawnActor<AGLandMine>(LandMineClass, SpawnedLocation,
																	 FRotator::ZeroRotator);
	SpawnedLandMine->SetOwner(GetController());
}

bool AGPlayerCharacter::SpawnLandMine_Server_Validate()
{
	return true;
}

void AGPlayerCharacter::OnRep_WeaponInstance()
{
	// Other에게 복제될 때
	// 호출되는 부분
	// NetMulticast로 해주지 않고 여기서 처리되도록 구현
	if (IsValid(WeaponInstance) == true)
	{
		TSubclassOf<UAnimInstance> WeaponCharacterAnimLayer = WeaponInstance->GetArmedCharacterAnimLayer();
		if (IsValid(WeaponCharacterAnimLayer) == true)
		{
			GetMesh()->LinkAnimClassLayers(WeaponCharacterAnimLayer);
		}

		TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance) == true)
		{
			if (WeaponInstance->GetWeaponNumber() == static_cast<int32>(EWeaponType::GreatSword))
			{
				AnimInstance->SetWeaponType(EWeaponType::GreatSword);
			}
			else if (WeaponInstance->GetWeaponNumber() == static_cast<int32>(EWeaponType::Bow))
			{
				AnimInstance->SetWeaponType(EWeaponType::Bow);
			}
		}
	}
	else
	{
		TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer = PlayerUnarmedCharacterAnimLayer;
		if (IsValid(UnarmedCharacterAnimLayer) == true)
		{
			GetMesh()->LinkAnimClassLayers(UnarmedCharacterAnimLayer);
		}

		TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance) == true)
		{
			AnimInstance->SetWeaponType(EWeaponType::None);
		}
	}
}

void AGPlayerCharacter::SpawnWeaponInstance_Server_Implementation(const int32& InWeaponNumber)
{
	FName WeaponSocket(TEXT("RightHandWeaponSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket) == true && IsValid(WeaponInstance) == false)
	{
		if(InWeaponNumber == 1)
		{
			WeaponInstance = GetWorld()->SpawnActor<AGWeaponActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
			if (IsValid(WeaponInstance) == true)
			{
				WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
												  WeaponSocket);
			}
		}
		else
		{
			WeaponInstance = GetWorld()->SpawnActor<AGWeaponActor>(WeaponClass2, FVector::ZeroVector, FRotator::ZeroRotator);
			if (IsValid(WeaponInstance) == true)
			{
				WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
												  WeaponSocket);
			}
		}
		
		TSubclassOf<UAnimInstance> WeaponCharacterAnimLayer = WeaponInstance->GetArmedCharacterAnimLayer();
		if (IsValid(WeaponCharacterAnimLayer) == true)
		{
			GetMesh()->LinkAnimClassLayers(WeaponCharacterAnimLayer);
		}

		if (TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			if(InWeaponNumber == 1)
			{
				AnimInstance->SetWeaponType(EWeaponType::GreatSword);
			}
			else
			{
				AnimInstance->SetWeaponType(EWeaponType::Bow);
			}
		}
	}

	SpawnWeaponInstance_NetMulticast(InWeaponNumber);
}

void AGPlayerCharacter::SpawnWeaponInstance_NetMulticast_Implementation(const int32& InWeaponNumber)
{
	if (HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
	// TSubclassOf<UAnimInstance> WeaponCharacterAnimLayer = WeaponInstance->GetArmedCharacterAnimLayer();
	// if (IsValid(WeaponCharacterAnimLayer) == true)
	// {
	// 	GetMesh()->LinkAnimClassLayers(WeaponCharacterAnimLayer);
	// }
	//
	// if (TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance()))
	// {
	// 	if(InWeaponNumber == 1)
	// 	{
	// 		AnimInstance->SetWeaponType(EWeaponType::GreatSword);
	// 	}
	// 	else
	// 	{
	// 		AnimInstance->SetWeaponType(EWeaponType::Bow);
	// 	}
	// }
	
	// 후 애니메이션 재생
	// WeaponInstance보단 WeaponClass DefaultObject 활용
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid((AnimInstance)) == true && IsValid((WeaponClass)) == true)
	{
		if (TObjectPtr<AGWeaponActor> DefaultWeapon = WeaponClass->GetDefaultObject<AGWeaponActor>())
		{
			//AnimInstance->PlayAnimMontage(DefaultWeapon->GetEquipAnimMontage());
		}
	}
}

void AGPlayerCharacter::DestroyWeaponInstance_Server_Implementation()
{
	// TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer = PlayerUnarmedCharacterAnimLayer;
	// if (IsValid(UnarmedCharacterAnimLayer) == true)
	// {
	// 	GetMesh()->LinkAnimClassLayers(UnarmedCharacterAnimLayer);
	// }
	//
	// if (TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance()))
	// {
	// 	AnimInstance->SetWeaponType(EWeaponType::None);
	// }

	if (IsValid(WeaponInstance) == true)
	{
		WeaponInstance->Destroy();
		WeaponInstance = nullptr;
	}
	
	DestroyWeaponInstance_NetMulticast();
}

void AGPlayerCharacter::DestroyWeaponInstance_NetMulticast_Implementation()
{
	if (HasAuthority() == true || IsLocallyControlled() == true)
		return;

	// TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer = PlayerUnarmedCharacterAnimLayer;
	// if (IsValid(UnarmedCharacterAnimLayer) == true)
	// {
	// 	GetMesh()->LinkAnimClassLayers(UnarmedCharacterAnimLayer);
	// }
	
	// 후 애니메이션 재생
	// WeaponInstance는 이미 Destroy되었기에 WeaponClass DefaultObject 활용
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid((AnimInstance)) == true && IsValid((WeaponClass)) == true)
	{
		// 글라이딩으로 인해 무기 해제될 때는 몽타주 재생 X
		if (bIsGliding == true)
			return;

		if (TObjectPtr<AGWeaponActor> DefaultWeapon = WeaponClass->GetDefaultObject<AGWeaponActor>())
		{
			//AnimInstance->PlayAnimMontage(DefaultWeapon->GetUnequipAnimMontage());
		}
	}
}

void AGPlayerCharacter::OnRep_GliderInstance()
{
}

void AGPlayerCharacter::BeginBasicAttackCombo()
{
	if (InputDirectionVector.IsNearlyZero() == false)
	{
		FRotator InputRotation = InputDirectionVector.Rotation();
		this->SetActorRotation(InputRotation);
		UpdateRotation_Server(InputRotation);
	}
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> BasicAttackAnimMontage = CurrentLinkedAnimInstance->GetBasicAttackAnimMontage();

	AnimInstance->PlayAnimMontage(BasicAttackAnimMontage);

	//GetCharacterMovement()->SetMovementMode(MOVE_None);
	bIsBasicAttacking = true;
	CurrentComboCount = 1;

	if (OnBasicAttackComboMontageEndedDelegate.IsBound() == false)
	{
		OnBasicAttackComboMontageEndedDelegate.BindUObject(this, &ThisClass::EndBasicAttackCombo);
		AnimInstance->Montage_SetEndDelegate(OnBasicAttackComboMontageEndedDelegate, BasicAttackAnimMontage);
	}

	BeginBasicAttackCombo_Server();
}

void AGPlayerCharacter::BeginBasicAttackCombo_Server_Implementation()
{
	bIsBasicAttacking = true;

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> BasicAttackAnimMontage = CurrentLinkedAnimInstance->GetBasicAttackAnimMontage();

	AnimInstance->PlayAnimMontage(BasicAttackAnimMontage);
	
	BeginBasicAttackCombo_NetMulticast();
}

void AGPlayerCharacter::BeginBasicAttackCombo_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> BasicAttackAnimMontage = CurrentLinkedAnimInstance->GetBasicAttackAnimMontage();

	AnimInstance->PlayAnimMontage(BasicAttackAnimMontage);
}

void AGPlayerCharacter::EndBasicAttackCombo(UAnimMontage* InMontage, bool bInterrupted)
{
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> BasicAttackAnimMontage = CurrentLinkedAnimInstance->GetBasicAttackAnimMontage();
	
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(BasicAttackAnimMontage))
	{
		AnimInstance->Montage_Stop(0.2f);
	}
	
	//ensureMsgf(CurrentComboCount != 0, TEXT("CurrentComboCount == 0"));
	CurrentComboCount = 0;
	bIsAttackKeyPressed = false;
	bIsBasicAttacking = false;
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if (OnBasicAttackComboMontageEndedDelegate.IsBound() == true)
	{
		OnBasicAttackComboMontageEndedDelegate.Unbind();
	}

	EndBasicAttackCombo_Server();
}

void AGPlayerCharacter::EndBasicAttackCombo_Server_Implementation()
{
	bIsBasicAttacking = false;

	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> BasicAttackAnimMontage = CurrentLinkedAnimInstance->GetBasicAttackAnimMontage();
	
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(BasicAttackAnimMontage))
	{
		AnimInstance->Montage_Stop(0.2f);
	}
	
	EndBasicAttackCombo_NetMulticast();
}

void AGPlayerCharacter::EndBasicAttackCombo_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> BasicAttackAnimMontage = CurrentLinkedAnimInstance->GetBasicAttackAnimMontage();
	
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(BasicAttackAnimMontage))
	{
		AnimInstance->Montage_Stop(0.2f);
	}
}

void AGPlayerCharacter::ChargedAttack_Owner()
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	if(AnimInstance->GetWeaponType() == EWeaponType::Bow)
	{
		bIsAiming = true;
	}
	else
	{
		bIsAiming = false;
		
		// LinkedAnimInstance의 AnimMontage 가져오기
		UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
		TObjectPtr<UAnimMontage> ChargedAttackAnimMontage = CurrentLinkedAnimInstance->GetChargedAttackAnimMontage();
		ensureMsgf(IsValid(ChargedAttackAnimMontage), TEXT("Invalid ChargedAttackAnimMontage"));

		AnimInstance->PlayAnimMontage(ChargedAttackAnimMontage);
	
		if (OnChargedAttackMontageEndedDelegate.IsBound() == false)
		{
			OnChargedAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndChargedAttack_Owner);
			AnimInstance->Montage_SetEndDelegate(OnChargedAttackMontageEndedDelegate, ChargedAttackAnimMontage);
		}
	}
	
	ChargedAttack_Server(bIsAiming);
}

void AGPlayerCharacter::ChargedAttack_Server_Implementation(const bool InIsAiming)
{
	bIsChargedAttacking = true;

	if(InIsAiming == true)
	{
		
	}
	else
	{
		TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

		// LinkedAnimInstance의 AnimMontage 가져오기
		UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
		TObjectPtr<UAnimMontage> ChargedAttackAnimMontage = CurrentLinkedAnimInstance->GetChargedAttackAnimMontage();
		ensureMsgf(IsValid(ChargedAttackAnimMontage), TEXT("Invalid ChargedAttackAnimMontage"));

		AnimInstance->PlayAnimMontage(ChargedAttackAnimMontage);
	}
	
	ChargedAttack_NetMulticast(InIsAiming);
}

void AGPlayerCharacter::ChargedAttack_NetMulticast_Implementation(const bool InIsAiming)
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;

	if(InIsAiming == true)
	{
		
	}
	else
	{
		TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

		// LinkedAnimInstance의 AnimMontage 가져오기
		UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
		TObjectPtr<UAnimMontage> ChargedAttackAnimMontage = CurrentLinkedAnimInstance->GetChargedAttackAnimMontage();
		ensureMsgf(IsValid(ChargedAttackAnimMontage), TEXT("Invalid ChargedAttackAnimMontage"));

		AnimInstance->PlayAnimMontage(ChargedAttackAnimMontage);
	}
}

void AGPlayerCharacter::EndChargedAttack_Owner(UAnimMontage* Montage, bool bInterrupted)
{
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> ChargedAttackAnimMontage = CurrentLinkedAnimInstance->GetChargedAttackAnimMontage();
	
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(ChargedAttackAnimMontage))
	{
		AnimInstance->Montage_Stop(0.2f);
	}

	if (OnChargedAttackMontageEndedDelegate.IsBound() == true)
	{
		OnChargedAttackMontageEndedDelegate.Unbind();
	}

	bIsChargedAttacking = false;

	EndChargedAttack_Server();
}

void AGPlayerCharacter::EndChargedAttack_Server_Implementation()
{
	bIsChargedAttacking = false;
	
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> ChargedAttackAnimMontage = CurrentLinkedAnimInstance->GetChargedAttackAnimMontage();
	
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(ChargedAttackAnimMontage))
	{
		AnimInstance->Montage_Stop(0.2f);
	}
	
	EndChargedAttack_NetMulticast();
}

void AGPlayerCharacter::EndChargedAttack_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> ChargedAttackAnimMontage = CurrentLinkedAnimInstance->GetChargedAttackAnimMontage();
	
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(ChargedAttackAnimMontage))
	{
		AnimInstance->Montage_Stop(0.2f);
	}
}

void AGPlayerCharacter::AirAttack_Owner()
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> AirAttackAnimMontage = CurrentLinkedAnimInstance->GetAirAttackAnimMontage();
	ensureMsgf(IsValid(AirAttackAnimMontage), TEXT("Invalid AirAttackAnimMontage"));
		
	AnimInstance->PlayAnimMontage(AirAttackAnimMontage);

	if (OnAirAttackMontageEndedDelegate.IsBound() == false)
	{
		OnAirAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAirAttack_Owner);
		AnimInstance->Montage_SetEndDelegate(OnAirAttackMontageEndedDelegate, AirAttackAnimMontage);
	}
	
	AirAttack_Server();
}

void AGPlayerCharacter::AirAttack_Server_Implementation()
{
	bIsAirAttacking = true;
	
	FVector DiagonalVector = -GetActorUpVector();
	LaunchCharacter(DiagonalVector * AirAttackLaunchVelocity, false, true);
	
	AirAttack_NetMulticast();
}

void AGPlayerCharacter::AirAttack_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> AirAttackAnimMontage = CurrentLinkedAnimInstance->GetAirAttackAnimMontage();
	ensureMsgf(IsValid(AirAttackAnimMontage), TEXT("Invalid AirAttackAnimMontage"));

	AnimInstance->PlayAnimMontage(AirAttackAnimMontage);
}

void AGPlayerCharacter::LastSectionAirAttack_Owner()
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> AirAttackAnimMontage = CurrentLinkedAnimInstance->GetAirAttackAnimMontage();
	ensureMsgf(IsValid(AirAttackAnimMontage), TEXT("Invalid AirAttackAnimMontage"));

	if (AnimInstance->Montage_IsPlaying(AirAttackAnimMontage))// 마지막 섹션으로 전환하는 경우
	{
		FName NextSectionName = *FString::Printf(TEXT("%s"), *AirAttackAnimMontageSectionName);
		AnimInstance->Montage_JumpToSection(NextSectionName, AirAttackAnimMontage);
	
		LastSectionAirAttack_Server();
	}
}

void AGPlayerCharacter::LastSectionAirAttack_Server_Implementation()
{
	UKismetSystemLibrary::PrintString(this, TEXT("LastSectionAirAttack_Server is called"));
	
	LastSectionAirAttack_NetMulticast();
}

void AGPlayerCharacter::LastSectionAirAttack_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> AirAttackAnimMontage = CurrentLinkedAnimInstance->GetAirAttackAnimMontage();
	ensureMsgf(IsValid(AirAttackAnimMontage), TEXT("Invalid AirAttackAnimMontage"));

	if (AnimInstance->Montage_IsPlaying(AirAttackAnimMontage))
	{
		FName NextSectionName = *FString::Printf(TEXT("%s"), *AirAttackAnimMontageSectionName);
		AnimInstance->Montage_JumpToSection(NextSectionName, AirAttackAnimMontage);
	}
}

void AGPlayerCharacter::EndAirAttack_Owner(UAnimMontage* Montage, bool bInterrupted)
{
	if (OnAirAttackMontageEndedDelegate.IsBound() == true)
	{
		OnAirAttackMontageEndedDelegate.Unbind();
	}
	
	EndAirAttack_Server();
}

void AGPlayerCharacter::EndAirAttack_Server_Implementation()
{
	//UKismetSystemLibrary::PrintString(this, TEXT("EndAirAttack_Server is called"));
	
	bIsAirAttacking = false;
	
	EndAirAttack_NetMulticast();
}

void AGPlayerCharacter::EndAirAttack_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
}

void AGPlayerCharacter::RunAttack_Owner()
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> RunAttackAnimMontage = CurrentLinkedAnimInstance->GetRunAttackAnimMontage();
	ensureMsgf(IsValid(RunAttackAnimMontage), TEXT("Invalid RunAttackAnimMontage"));
		
	AnimInstance->PlayAnimMontage(RunAttackAnimMontage);

	if (OnRunAttackMontageEndedDelegate.IsBound() == false)
	{
		OnRunAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndRunAttack_Owner);
		AnimInstance->Montage_SetEndDelegate(OnRunAttackMontageEndedDelegate, RunAttackAnimMontage);
	}
	
	RunAttack_Server();
}

void AGPlayerCharacter::RunAttack_Server_Implementation()
{
	bIsRunAttacking = true;

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> RunAttackAnimMontage = CurrentLinkedAnimInstance->GetRunAttackAnimMontage();
	ensureMsgf(IsValid(RunAttackAnimMontage), TEXT("Invalid RunAttackAnimMontage"));
	
	AnimInstance->PlayAnimMontage(RunAttackAnimMontage);
	
	RunAttack_NetMulticast();
}

void AGPlayerCharacter::RunAttack_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> RunAttackAnimMontage = CurrentLinkedAnimInstance->GetRunAttackAnimMontage();
	ensureMsgf(IsValid(RunAttackAnimMontage), TEXT("Invalid RunAttackAnimMontage"));
		
	AnimInstance->PlayAnimMontage(RunAttackAnimMontage);
}

void AGPlayerCharacter::EndRunAttack_Owner(UAnimMontage* Montage, bool bInterrupted)
{
	if (OnRunAttackMontageEndedDelegate.IsBound() == true)
	{
		OnRunAttackMontageEndedDelegate.Unbind();
	}

	EndRunAttack_Server();
}

void AGPlayerCharacter::EndRunAttack_Server_Implementation()
{
	bIsRunAttacking = false;
	
	EndRunAttack_NetMulticast();
}

void AGPlayerCharacter::EndRunAttack_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
}
void AGPlayerCharacter::CrouchAttack_Owner()
{
	// 일어나면서
	CrouchChange_Owner();

	// 찌르는 몽타주 재생
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> CrouchAttackAnimMontage = CurrentLinkedAnimInstance->GetCrouchAttackAnimMontage();
	ensureMsgf(IsValid(CrouchAttackAnimMontage), TEXT("Invalid CrouchAttackAnimMontage"));
		
	AnimInstance->PlayAnimMontage(CrouchAttackAnimMontage);

	if (OnCrouchAttackMontageEndedDelegate.IsBound() == false)
	{
		OnCrouchAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndCrouchAttack_Owner);
		AnimInstance->Montage_SetEndDelegate(OnCrouchAttackMontageEndedDelegate, CrouchAttackAnimMontage);
	}
	
	CrouchAttack_Server();
}

void AGPlayerCharacter::CrouchAttack_Server_Implementation()
{
	CrouchChange_Server();

	bIsCrouchAttacking = true;

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> CrouchAttackAnimMontage = CurrentLinkedAnimInstance->GetCrouchAttackAnimMontage();
	ensureMsgf(IsValid(CrouchAttackAnimMontage), TEXT("Invalid CrouchAttackAnimMontage"));
		
	AnimInstance->PlayAnimMontage(CrouchAttackAnimMontage);
	
	CrouchAttack_NetMulticast();
}

void AGPlayerCharacter::CrouchAttack_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> CrouchAttackAnimMontage = CurrentLinkedAnimInstance->GetCrouchAttackAnimMontage();
	ensureMsgf(IsValid(CrouchAttackAnimMontage), TEXT("Invalid CrouchAttackAnimMontage"));
		
	AnimInstance->PlayAnimMontage(CrouchAttackAnimMontage);
}

void AGPlayerCharacter::EndCrouchAttack_Owner(UAnimMontage* Montage, bool bInterrupted)
{
	if (OnCrouchAttackMontageEndedDelegate.IsBound() == true)
	{
		OnCrouchAttackMontageEndedDelegate.Unbind();
	}
	
	EndCrouchAttack_Server();
}

void AGPlayerCharacter::EndCrouchAttack_Server_Implementation()
{
	bIsCrouchAttacking = false;
	
	EndCrouchAttack_NetMulticast();
}

void AGPlayerCharacter::EndCrouchAttack_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
}

void AGPlayerCharacter::SkillFirst_Owner()
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> SkillFirstAnimMontage = CurrentLinkedAnimInstance->GetSkillFirstAnimMontage();
	ensureMsgf(IsValid(SkillFirstAnimMontage), TEXT("Invalid SkillFirstAnimMontage"));
		
	AnimInstance->PlayAnimMontage(SkillFirstAnimMontage);

	if (OnSkillFirstAttackMontageEndedDelegate.IsBound() == false)
	{
		OnSkillFirstAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndSkillFirstAttack_Owner);
		AnimInstance->Montage_SetEndDelegate(OnSkillFirstAttackMontageEndedDelegate, SkillFirstAnimMontage);
	}

	SkillFirst_Server();
}

void AGPlayerCharacter::SkillFirst_Server_Implementation()
{
	bIsSkillFirstAttacking = true;

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> SkillFirstAnimMontage = CurrentLinkedAnimInstance->GetSkillFirstAnimMontage();
	ensureMsgf(IsValid(SkillFirstAnimMontage), TEXT("Invalid SkillFirstAnimMontage"));
		
	AnimInstance->PlayAnimMontage(SkillFirstAnimMontage);

	SkillFirst_NetMulticast();
}

void AGPlayerCharacter::SkillFirst_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> SkillFirstAnimMontage = CurrentLinkedAnimInstance->GetSkillFirstAnimMontage();
	ensureMsgf(IsValid(SkillFirstAnimMontage), TEXT("Invalid SkillFirstAnimMontage"));
		
	AnimInstance->PlayAnimMontage(SkillFirstAnimMontage);
}

void AGPlayerCharacter::EndSkillFirstAttack_Owner(UAnimMontage* Montage, bool bInterrupted)
{
	if (OnSkillFirstAttackMontageEndedDelegate.IsBound() == true)
	{
		OnSkillFirstAttackMontageEndedDelegate.Unbind();
	}
	
	EndSkillFirstAttack_Server();
}

void AGPlayerCharacter::EndSkillFirstAttack_Server_Implementation()
{
	bIsSkillFirstAttacking = false;

	EndSkillFirstAttack_NetMulticast();
}

void AGPlayerCharacter::EndSkillFirstAttack_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
}

void AGPlayerCharacter::SkillSecond_Owner()
{
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> SkillSecondAnimMontage = CurrentLinkedAnimInstance->GetSkillSecondAnimMontage();
	ensureMsgf(IsValid(SkillSecondAnimMontage), TEXT("Invalid SkillSecondAnimMontage"));
	
	AnimInstance->PlayAnimMontage(SkillSecondAnimMontage);

	if (OnSkillSecondAttackMontageEndedDelegate.IsBound() == false)
	{
		OnSkillSecondAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndSkillSecondAttack_Owner);
		AnimInstance->Montage_SetEndDelegate(OnSkillSecondAttackMontageEndedDelegate, SkillSecondAnimMontage);
	}

	SkillSecond_Server();
}

void AGPlayerCharacter::SkillSecond_Server_Implementation()
{
	bIsSkillSecondAttacking = true;

	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> SkillSecondAnimMontage = CurrentLinkedAnimInstance->GetSkillSecondAnimMontage();
	ensureMsgf(IsValid(SkillSecondAnimMontage), TEXT("Invalid SkillSecondAnimMontage"));
	
	AnimInstance->PlayAnimMontage(SkillSecondAnimMontage);

	SkillSecond_NetMulticast();
}

void AGPlayerCharacter::SkillSecond_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> SkillSecondAnimMontage = CurrentLinkedAnimInstance->GetSkillSecondAnimMontage();
	ensureMsgf(IsValid(SkillSecondAnimMontage), TEXT("Invalid SkillSecondAnimMontage"));
		
	AnimInstance->PlayAnimMontage(SkillSecondAnimMontage);
}

void AGPlayerCharacter::EndSkillSecondAttack_Owner(UAnimMontage* Montage, bool bInterrupted)
{
	if (OnSkillSecondAttackMontageEndedDelegate.IsBound() == true)
	{
		OnSkillSecondAttackMontageEndedDelegate.Unbind();
	}
	
	EndSkillSecondAttack_Server();
}

void AGPlayerCharacter::EndSkillSecondAttack_Server_Implementation()
{
	bIsSkillSecondAttacking = false;
	
	EndSkillSecondAttack_NetMulticast();
}

void AGPlayerCharacter::EndSkillSecondAttack_NetMulticast_Implementation()
{
	if(HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
}

void AGPlayerCharacter::OnCheckAttackInput_Server_Implementation(const uint8& InbIsAttackKeyPressed, const int32& InCurrentComboCount)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnCheckAttackInput_Server is called")));
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> BasicAttackAnimMontage = CurrentLinkedAnimInstance->GetBasicAttackAnimMontage();
	ensureMsgf(IsValid(BasicAttackAnimMontage), TEXT("Invalid BasicAttackAnimMontage"));
	
	if (InbIsAttackKeyPressed == static_cast<uint8>(true))
	{
		//CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

		FName NextSectionName = *FString::Printf(TEXT("%s%d"), *AttackAnimMontageSectionName, InCurrentComboCount);
		AnimInstance->Montage_JumpToSection(NextSectionName, BasicAttackAnimMontage);
		
		//bIsAttackKeyPressed = false;
	}
	
	OnCheckAttackInput_NetMulticast(InbIsAttackKeyPressed, InCurrentComboCount);
}

void AGPlayerCharacter::OnCheckAttackInput_NetMulticast_Implementation(const uint8& InbIsAttackKeyPressed, const int32& InCurrentComboCount)
{
	if(HasAuthority() == true|| IsLocallyControlled() == true)
		return;
	
	TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	// LinkedAnimInstance의 AnimMontage 가져오기
	UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
	TObjectPtr<UAnimMontage> BasicAttackAnimMontage = CurrentLinkedAnimInstance->GetBasicAttackAnimMontage();
	ensureMsgf(IsValid(BasicAttackAnimMontage), TEXT("Invalid BasicAttackAnimMontage"));
	
	if (InbIsAttackKeyPressed == static_cast<uint8>(true))
	{
		//CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

		FName NextSectionName = *FString::Printf(TEXT("%s%d"), *AttackAnimMontageSectionName, InCurrentComboCount);
		AnimInstance->Montage_JumpToSection(NextSectionName, BasicAttackAnimMontage);
		
		//bIsAttackKeyPressed = false;
	}
}

void AGPlayerCharacter::ApplyDamageAndDrawLine_Server_Implementation(FHitResult HitResult, const bool bResult)
{
	if(bResult == true)
	{
		AGCharacter* HittedCharacter = Cast<AGCharacter>(HitResult.GetActor());
        	//AGMonster* HittedCharacter = Cast<AGMonster>(HitResult.GetActor());
        	if (IsValid(HittedCharacter) == true)
        	{
        		//FDamageEvent DamageEvent;
        		
        		// FString BoneNameString = HitResult.BoneName.ToString();
        		// if (true == BoneNameString.Equals(FString(TEXT("HEAD")), ESearchCase::IgnoreCase))
        		// {
        		// 	HittedCharacter->TakeDamage(100.f, DamageEvent, GetController(), this);
        		// }
        		// else
        		{
        			//UKismetSystemLibrary::PrintString(this, TEXT("TakeDamage is called"));
        			
        			FDamageEvent DamageEvent;
        			HittedCharacter->TakeDamage(10.f, DamageEvent, GetController(), this);
        		}
        	}
	}
	
	DrawLine_NetMulticast(bResult);
}

void AGPlayerCharacter::DrawLine_NetMulticast_Implementation(const bool bResult)
{
	if (HasAuthority() == true)
		return;

	FVector TraceVector = BasicAttackRange * GetActorForwardVector();
	FVector Center = GetActorLocation() + TraceVector + GetActorUpVector() * 40.f;
	float HalfHeight = BasicAttackRange * 0.5f + BasicAttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.f;

	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		BasicAttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);
}

void AGPlayerCharacter::SpawnGliderInstance_Server_Implementation(const bool bIsFirst)
{
	// 글라이더 장착
	FName GliderSocket(TEXT("GliderSocket"));
	if (GetMesh()->DoesSocketExist(GliderSocket) == true && IsValid(GliderInstance) == false)
	{
		GliderInstance = GetWorld()->SpawnActor<AGGliderActor>(
			GliderClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (IsValid(GliderInstance) == true)
		{
			GliderInstance->AttachToComponent(
				GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GliderSocket);
		}
	}

	bIsGliding = true;

	SpawnGliderInstance_NetMulticast(bIsFirst);
}

void AGPlayerCharacter::SpawnGliderInstance_NetMulticast_Implementation(const bool bIsFirst)
{
	if (HasAuthority() == true || IsLocallyControlled() == true)
		return;
	
	if(bIsFirst == true)
	{
		TObjectPtr<UGAnimInstance> AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
		
		// LinkedAnimInstance의 AnimMontage 가져오기
		UGAnimInstance* CurrentLinkedAnimInstance = GetLinkedAnimInstance();
		TObjectPtr<UAnimMontage> GlidingStartAnimMontage = CurrentLinkedAnimInstance->GetGlidingStartAnimMontage();
		ensureMsgf(IsValid(GlidingStartAnimMontage), TEXT("Invalid GlidingStartAnimMontage"));
		
		AnimInstance->PlayAnimMontage(GlidingStartAnimMontage);
	}
	
	// 글라이더 장착 애님 몽타주 미사용
	//if (IsValid(GliderInstance->GetEquipAnimMontage()))
	//{
	//	AnimInstance->PlayAnimMontage(GliderInstance->GetEquipAnimMontage());
	//	AnimInstance->SetWeaponType(EWeaponType::None);
	//}
}

void AGPlayerCharacter::DestroyGliderInstance_Server_Implementation()
{
	if (IsValid(GliderInstance) == true)
	{
		GliderInstance->Destroy();
		GliderInstance = nullptr;
	}

	// 글라이더 미장착 움직임 적용
	GetCharacterMovement()->GravityScale = 1.75f;
	GetCharacterMovement()->AirControl = 0.35f;

	// 인풋 초기화
	RightInputValue = 0.0f;
	ForwardInputValue = 0.0f;
	InputDirectionVector = FVector::ZeroVector;

	UpdateInputValue_Server(ForwardInputValue, RightInputValue);
	UpdateInputDirectionVector_Server(InputDirectionVector);

	bIsGliding = false;
	
	DestroyGliderInstance_NetMulticast();
}

void AGPlayerCharacter::DestroyGliderInstance_NetMulticast_Implementation()
{
	if (HasAuthority() == true || IsLocallyControlled() == true)
		return;

	// 글라이더 탈거 애님 몽타주 미사용
	//if (IsValid(GliderInstance->GetEquipAnimMontage()))
	//{
	//	AnimInstance->PlayAnimMontage(GliderInstance->GetUnEquipAnimMontage());
	//	AnimInstance->SetWeaponType(EWeaponType::None);
	//}
}

UE_ENABLE_OPTIMIZATION
