// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayableCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/GInputConfig.h"
#include "Kismet/KismetSystemLibrary.h"

APlayableCharacter::APlayableCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void APlayableCharacter::BeginPlay()
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

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
		
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator); 
	// ControlRotation이 Pawn의 회전과 동기화 -> Pawn의 회전이 SpringArm의 회전 동기화. 이로 인해 SetRotation()이 무의미.

	SpringArmComponent->bUsePawnControlRotation = true;
		
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;
		
	SpringArmComponent->bDoCollisionTest = true;
	
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent) == true)
	{
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Look, ETriggerEvent::Triggered, this, &ThisClass::InputLook);
	}
}

void APlayableCharacter::InputMove(const FInputActionValue& InValue)
{
	FVector2D MovementVector = InValue.Get<FVector2D>();

	const FRotator ControlRotation = GetController()->GetControlRotation();
	const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);

	const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVector, MovementVector.X);
	AddMovementInput(RightVector, MovementVector.Y);
}

void APlayableCharacter::InputLook(const FInputActionValue& InValue)
{
	if (IsValid(GetController()) == true)
	{
		FVector2D LookVector = InValue.Get<FVector2D>();

		AddControllerYawInput(LookVector.X); 
		AddControllerPitchInput(LookVector.Y);
	}
}
