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

				//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%d"), (int)MovementDirection));

				if (ForwardValue > KINDA_SMALL_NUMBER)
				{
					MovementDirection = EMovementDirection::Fwd;

					if (RightValue > KINDA_SMALL_NUMBER)
					{
						MovementDirection = EMovementDirection::RightFwd;
					}
					else if (RightValue < -KINDA_SMALL_NUMBER)
					{
						MovementDirection = EMovementDirection::LeftFwd;
					}
				}
				else if (ForwardValue < -KINDA_SMALL_NUMBER)
				{
					MovementDirection = EMovementDirection::Bwd;

					if (RightValue > KINDA_SMALL_NUMBER)
					{
						MovementDirection = EMovementDirection::RightBwd;
					}
					else if (RightValue < -KINDA_SMALL_NUMBER)
					{
						MovementDirection = EMovementDirection::LeftBwd;
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
						if (ForwardValue < -KINDA_SMALL_NUMBER)
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
				else// 못 달리는 상태
				{
					bIsRunning = false;

					// 뒤 방향일 때만 속도 줄이기
					if (ForwardValue < -KINDA_SMALL_NUMBER)
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
