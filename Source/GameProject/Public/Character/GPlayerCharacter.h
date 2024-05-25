// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GCharacter.h"
#include "InputActionValue.h"
#include "GPlayerCharacter.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EViewMode : uint8
{
	None,
	BackCombatView,
	BackGeneralView,
	End
};

UCLASS()
class GAMEPROJECT_API AGPlayerCharacter : public AGCharacter
{
	GENERATED_BODY()

public:
	AGPlayerCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	void SetViewMode(EViewMode InViewMode);

	EViewMode GetViewMode() const { return CurrentViewMode; }

	float GetForwardInputValue() const { return ForwardInputValue; }

	float GetRightInputValue() const { return RightInputValue; }

	bool IsInputRun() const { return bIsInputRun; }

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);
	void InputJumpStart(const FInputActionValue& InValue);
	void InputJumpEnd(const FInputActionValue& InValue);
	void InputEquip(const FInputActionValue& InValue);
	void InputUnEquip(const FInputActionValue& InValue);
	void InputRunStart(const FInputActionValue& InValue);
	void InputRunEnd(const FInputActionValue& InValue);
	void InputChangeView(const FInputActionValue& InValue);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class UGInputConfig> PlayerCharacterInputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	float ForwardInputValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	float RightInputValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class UCurveFloat> DirectionCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	float RotationInterpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	float BaseLookUpRate;

	FVector InputDirectionVector = FVector::ZeroVector;

	EViewMode CurrentViewMode = EViewMode::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TSubclassOf<class AGWeaponActor> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class AGWeaponActor> WeaponInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	uint8 bIsInputRun : 1;

	int32 MaxJumpCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TSubclassOf<class AGGliderActor> GliderClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class AGGliderActor> GliderInstance;
};
