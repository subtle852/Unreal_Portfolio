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
	FwdLeft,
	FwdRight,
	BwdLeft,
	BwdRight,
	End
};

UENUM(BlueprintType)
enum class EAnimMoveType : uint8
{
	None,
	UnLock,
	Lock,
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckAttackInput);

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

	EAnimMoveType GetAnimMoveType() const { return AnimMoveType; }
	void SetAnimMoveType(EAnimMoveType InAnimMoveMode) { AnimMoveType = InAnimMoveMode; }

	bool IsFalling() const { return bIsFalling; }
	bool IsGliding() const { return bIsGliding; }
	void SetGliding(int32 InIsGliding) { bIsGliding = InIsGliding; }
	bool IsCrouching() const {return bIsCrouching; }

	void PlayAnimMontage(UAnimMontage* InAnimMontage);

	TObjectPtr<UAnimMontage> GetJumpFlipAnimMontage() const;
	TObjectPtr<UAnimMontage> GetDashAnimMontage() const;
	TObjectPtr<UAnimMontage> GetBasicAttackAnimMontage() const;
	TObjectPtr<UAnimMontage> GetChargedAttackAnimMontage() const;
	TObjectPtr<UAnimMontage> GetAirAttackAnimMontage() const;
	TObjectPtr<UAnimMontage> GetRunAttackAnimMontage() const;
	TObjectPtr<UAnimMontage> GetCrouchAttackAnimMontage() const;

	void InitializeMainAnimInstance(UGAnimInstance* InMainAnimInstance) { MainAnimInstance = InMainAnimInstance; }

protected:
	UFUNCTION()
	void AnimNotify_CheckHit();

	UFUNCTION()
	void AnimNotify_CheckAttackInput();

public:
	FOnCheckHit OnCheckHit;
	FOnCheckAttackInput OnCheckAttackInput;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	UGAnimInstance* MainAnimInstance;

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
	uint8 bIsGliding : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	uint8 bIsDead : 1;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	ELocomotionState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	EMovementDirection MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAnimInstance", meta = (AllowPrivateAccess))
	EAnimMoveType AnimMoveType;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAnimInstance|Jump", meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> JumpFlipMontage;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAnimInstance|Dash", meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> DashFwdAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAnimInstance|Dash", meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> DashBwdAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAnimInstance|Dash", meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> DashLeftAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAnimInstance|Dash", meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> DashRightAnimMontage;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAnimInstance|Attack", meta = (AllowPrivateAccess))
	TObjectPtr< UAnimMontage> BasicAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAnimInstance|Attack", meta = (AllowPrivateAccess))
	TObjectPtr< UAnimMontage> ChargedAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAnimInstance|Attack", meta = (AllowPrivateAccess))
	TObjectPtr< UAnimMontage> AirAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAnimInstance|Attack", meta = (AllowPrivateAccess))
	TObjectPtr< UAnimMontage> RunAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAnimInstance|Attack", meta = (AllowPrivateAccess))
	TObjectPtr< UAnimMontage> CrouchAttackAnimMontage;
	
};
