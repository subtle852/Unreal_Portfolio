// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GAnimInstance.generated.h"

/**
 * 
 */

class UAnimMontage;
enum class EViewMode : uint8;

UENUM(BlueprintType)
enum class ELocomotionState : uint8
{
	None,
	Idle,
	Walk,
	// Jog,
	Pivoting,
	Jumping,
	End
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	None,
	Fwd,
	Bwd,
	Left,
	Right,
	//LeftFwd,
	//RightFwd,
	//LeftBwd,
	//RightBwd,
	End
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None,
	GreatSword,
	ShieldSword,
	Bow,
	Magic,
	End
};

UCLASS()
class GAMEPROJECT_API UGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UGAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	ELocomotionState GetLocomotionState() const { return LocomotionState; }

	EMovementDirection GetMovementDirection() const { return MovementDirection; }

	EWeaponType GetWeaponType() const { return WeaponType; }
	
	void SetWeaponType(EWeaponType InWeaponType) { WeaponType = InWeaponType; }

	void SetAnimCurrentViewMode(EViewMode InViewMode) { AnimCurrentViewMode = InViewMode; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	FVector Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	uint8 bIsCrouching : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	uint8 bIsRunning : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	ELocomotionState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	EMovementDirection MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	EViewMode AnimCurrentViewMode;
};
