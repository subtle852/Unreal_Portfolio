// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/GAnimInstance.h"
#include "Character/GPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UGAnimInstance::UGAnimInstance()
{
}

void UGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CurrentSpeed = 0.f;

	Velocity = FVector::ZeroVector;

	bIsFalling = false;

	bIsCrouching = false;

	bIsRunning = false;

	AnimCurrentViewMode = EViewMode::BackCombatView;

	CurrentJumpCount = 0;

	bIsGliding = false;
}

void UGAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	AGCharacter* OwnerCharacter = Cast<AGCharacter>(TryGetPawnOwner());

	if (IsValid(OwnerCharacter) == true)
	{
		UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
		if (IsValid(CharacterMovementComponent) == true)
		{
			Velocity = CharacterMovementComponent->GetLastUpdateVelocity();
			CurrentSpeed = Velocity.Size();
			bIsFalling = CharacterMovementComponent->IsFalling();
			bIsCrouching = CharacterMovementComponent->IsCrouching();
			Acceleration = CharacterMovementComponent->GetCurrentAcceleration();
			//bIsRunning = OwnerCharacter->bIsInputRun;
			//AnimWeaponType = OwnerCharacter->GetWeaponType();

			if (Acceleration.Length() < KINDA_SMALL_NUMBER && Velocity.Length() < KINDA_SMALL_NUMBER)
			{
				LocomotionState = ELocomotionState::Idle;
			}
			else
			{
				LocomotionState = ELocomotionState::Walk;
			}

			AGPlayerCharacter* OwnerPlayerCharacter = Cast<AGPlayerCharacter>(OwnerCharacter);
			if (IsValid(OwnerPlayerCharacter) == true)
			{
				const float ForwardValue = OwnerPlayerCharacter->GetForwardInputValue();
				const float RightValue = OwnerPlayerCharacter->GetRightInputValue();

				//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%d"), (int)AnimCurrentViewMode));

				if (ForwardValue <= KINDA_SMALL_NUMBER && RightValue <= KINDA_SMALL_NUMBER)
				{
					MovementDirection = EMovementDirection::None;
				}

				if (AnimCurrentViewMode == EViewMode::BackGeneralView && bIsGliding == false)
				{
					// 해당 움직임 뷰는 무조건 Fwd만 가능
					// 추후 수정이 필요하면 None도 가능하게 할 예정
					MovementDirection = EMovementDirection::Fwd;

					if (OwnerPlayerCharacter->IsInputRun() == true)
					{
						if (LocomotionState == ELocomotionState::Walk)
						{
							bIsRunning = true;// 달리는 상태

							OwnerCharacter->SetWalkSpeed(600.f);
						}
						else
						{
							bIsRunning = false;// 못 달리는 상태
						}
					}
					else// 안 달리는 상태
					{
						bIsRunning = false;
						OwnerCharacter->SetWalkSpeed(300.f);
					}
				}
				else if (AnimCurrentViewMode == EViewMode::BackCombatView || bIsGliding == true)
				{
					if (ForwardValue > KINDA_SMALL_NUMBER)
					{
						MovementDirection = EMovementDirection::Fwd;

						if (RightValue > KINDA_SMALL_NUMBER)
						{
							MovementDirection = EMovementDirection::FwdRight;
						}
						else if (RightValue < -KINDA_SMALL_NUMBER)
						{
							MovementDirection = EMovementDirection::FwdLeft;
						}
					}
					else if (ForwardValue < -KINDA_SMALL_NUMBER)
					{
						MovementDirection = EMovementDirection::Bwd;

						if (RightValue > KINDA_SMALL_NUMBER)
						{
							MovementDirection = EMovementDirection::BwdRight;
						}
						else if (RightValue < -KINDA_SMALL_NUMBER)
						{
							MovementDirection = EMovementDirection::BwdLeft;
						}
					}
					else
					{
						if (RightValue > KINDA_SMALL_NUMBER)
						{
							MovementDirection = EMovementDirection::Right;
						}
						else if (RightValue < -KINDA_SMALL_NUMBER)
						{
							MovementDirection = EMovementDirection::Left;
						}
					}

					if (OwnerPlayerCharacter->IsInputRun() == true)
					{
						if (LocomotionState == ELocomotionState::Walk)
						{
							bIsRunning = true;// 달리는 상태

							// 뒤 방향일 때만 속도 줄이기
							if (MovementDirection == EMovementDirection::Bwd)// BwdLeft, BwdRight도 추가해줘야 함 (대기중)
							{
								OwnerCharacter->SetWalkSpeed(300.f);
							}
							else
							{
								OwnerCharacter->SetWalkSpeed(600.f);
							}
						}
						else
						{
							bIsRunning = false;// 못 달리는 상태
						}
					}
					else// 안 달리는 상태
					{
						bIsRunning = false;

						// 뒤 방향일 때만 속도 줄이기
						if (MovementDirection == EMovementDirection::Bwd)// BwdLeft, BwdRight도 추가해줘야 함 (대기중)
						{
							OwnerCharacter->SetWalkSpeed(150.f);
						}
						else
						{
							OwnerCharacter->SetWalkSpeed(300.f);
						}
					}
				}
			}
		}
	}
}

void UGAnimInstance::PlayAnimMontage(UAnimMontage* InAnimMontage)
{
	ensureMsgf(IsValid(InAnimMontage) != false, TEXT("Invalid InAnimMontage"));

	if (Montage_IsPlaying(InAnimMontage) == false)
	{
		Montage_Play(InAnimMontage);
	}
}
