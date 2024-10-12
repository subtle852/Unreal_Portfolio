// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "GCharacter.generated.h"

UENUM(BlueprintType)
enum class ECheckHitDirection : uint8
{
	None,
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down,
	End
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None		UMETA(DisplayName = "None"),
	Basic		UMETA(DisplayName = "Basic"),
	Special		UMETA(DisplayName = "Special"),
	End			UMETA(DisplayName = "End")
};

USTRUCT()
struct FAttackDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

public:
	EAttackType AttackType;

	FAttackDamageEvent() : FDamageEvent(), AttackType(EAttackType::None) {}
	
	virtual const UScriptStruct* GetType() const
	{
		return FAttackDamageEvent::StaticStruct();
	}
};

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

	UFUNCTION(Server, Reliable)
	void SpawnBloodEffect_Server(const FHitResult& InHitResult);
	
	UFUNCTION(NetMulticast, Reliable)
	void SpawnBloodEffect_NetMulticast(const FHitResult& InHitResult);
	
	UFUNCTION()
	void OnBloodEffectFinished(UNiagaraComponent* FinishedComponent);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UGStatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess))
	TObjectPtr<class UPhysicalAnimationComponent> PhysicalAnimationComponent;

	// Blood Effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter|Blood", meta = (AllowPrivateAccess))
	TObjectPtr<class UNiagaraSystem> BloodNiagaraSystemTemplate;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGPlayerCharacter|Blood", meta = (AllowPrivateAccess))
	TArray<class UNiagaraComponent*> ActiveNiagaraComponents;
	
};

