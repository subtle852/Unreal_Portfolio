// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GCharacter.generated.h"

class UGStatComponent;
class UGWidget;

UCLASS()
class GAMEPROJECT_API AGCharacter : public ACharacter
{
	GENERATED_BODY()

	friend class UAN_CheckHit;

public:
	// Sets default values for this character's properties
	AGCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetWalkSpeed(float NewSpeed);
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UGStatComponent* GetStatComponent() const { return StatComponent; }

	virtual void SetWidget(UGWidget* InStudyWidget) {}

protected:
	UFUNCTION()
	void OnCharacterDeath();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UGStatComponent> StatComponent;
	
};
