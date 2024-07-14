// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GCharacter.h"
#include "InputActionValue.h"
#include "PlayableCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGInputConfig;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API APlayableCharacter : public AGCharacter
{
	GENERATED_BODY()
	
public:
	APlayableCharacter();

	virtual void BeginPlay() override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void InputMove(const FInputActionValue& InValue);

	void InputLook(const FInputActionValue& InValue);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UGInputConfig> PlayerCharacterInputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> PlayerCharacterInputMappingContext;
};
