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
	BackView,
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

	float GetForwardInputValue() const { return ForwardInputValue; }

	float GetRightInputValue() const { return RightInputValue; }

	bool IsInputRun() const { return bIsInputRun; }

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);
	void InputEquip(const FInputActionValue& InValue);
	void InputUnEquip(const FInputActionValue& InValue);
	void InputRunStart(const FInputActionValue& InValue);
	void InputRunEnd(const FInputActionValue& InValue);
	
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

	EViewMode CurrentViewMode = EViewMode::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TSubclassOf<class AGWeaponActor> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class AGWeaponActor> WeaponInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter", meta = (AllowPrivateAccess))
	uint8 bIsInputRun : 1;
};
