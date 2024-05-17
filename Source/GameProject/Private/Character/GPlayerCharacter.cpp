#include "Character/GPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/GInputConfig.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/GWeaponActor.h"
#include "Animation/GAnimInstance.h"

AGPlayerCharacter::AGPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
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
}

void AGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	//if(IsValid(AnimInstance) == true)
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f"), AnimInstance->Velocity.Z));

}

void AGPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetViewMode(EViewMode::BackView);
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
	case EViewMode::BackView:
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;// 동시에 시선방향과 캐릭터 회전이 동기화
		bUseControllerRotationRoll = false;

		SpringArmComponent->TargetArmLength = 400.f;
		SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);
		// ControlRotation을 Pawn의 회전과 동기화,
		// Pawn의 회전이 SpringArm의 회전과 동기화 상태이기에 SetRotation()이 무의미

		SpringArmComponent->bUsePawnControlRotation = true;

		SpringArmComponent->bInheritPitch = true;
		SpringArmComponent->bInheritYaw = true;
		SpringArmComponent->bInheritRoll = false;

		SpringArmComponent->bDoCollisionTest = true;

		GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
		GetCharacterMovement()->bOrientRotationToMovement = false;// 이동 키를 눌러야 동기화
		GetCharacterMovement()->bUseControllerDesiredRotation = true;

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
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Equip, ETriggerEvent::Started, this, &ThisClass::InputEquip);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->UnEquip, ETriggerEvent::Started, this, &ThisClass::InputUnEquip);
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
		case EViewMode::BackView:
		{
			const FRotator ControlRotation = GetController()->GetControlRotation();
			const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);

			const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
			const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

			AddMovementInput(ForwardVector, MovementVector.X);
			AddMovementInput(RightVector, MovementVector.Y);

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
		case EViewMode::BackView:
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


void AGPlayerCharacter::InputEquip(const FInputActionValue& InValue)
{
	FName WeaponSocket(TEXT("RightHandWeaponSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket) == true && IsValid(WeaponInstance) == false)
	{
		WeaponInstance = GetWorld()->SpawnActor<AGWeaponActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (IsValid(WeaponInstance) == true)
		{
			WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
		}

		TSubclassOf<UAnimInstance> RifleCharacterAnimLayer = WeaponInstance->GetArmedCharacterAnimLayer();
		if (IsValid(RifleCharacterAnimLayer) == true)
		{
			GetMesh()->LinkAnimClassLayers(RifleCharacterAnimLayer);
		}

		UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance) == true && IsValid(WeaponInstance->GetEquipAnimMontage()))
		{
			AnimInstance->Montage_Play(WeaponInstance->GetEquipAnimMontage());
		}
	}
}

void AGPlayerCharacter::InputUnEquip(const FInputActionValue& InValue)
{
	if (IsValid(WeaponInstance) == true)
	{
		TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer = WeaponInstance->GetUnarmedCharacterAnimLayer();
		if (IsValid(UnarmedCharacterAnimLayer) == true)
		{
			GetMesh()->LinkAnimClassLayers(UnarmedCharacterAnimLayer);
		}

		UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance) == true && IsValid(WeaponInstance->GetUnequipAnimMontage()))
		{
			AnimInstance->Montage_Play(WeaponInstance->GetUnequipAnimMontage());
		}

		WeaponInstance->Destroy();
		WeaponInstance = nullptr;
	}
}

