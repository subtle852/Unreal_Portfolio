// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOutOfCurrentHPDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentHPChangeDelegate, float, InOldCurrentHP, float, InNewCurrentHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxHPChangeDelegate, float, InOldMaxHP, float, InNewMaxHP);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPROJECT_API UGStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGStatComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitializeComponent() override;

	//virtual void Serialize(FArchive& Ar) override;// 수동 직렬화
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;// 수동 복제
	
	float GetMaxHP() const { return MaxHP; }

	void SetMaxHP(float InMaxHP);

	float GetCurrentHP() const { return CurrentHP; }

	void SetCurrentHP(float InCurrentHP);

	bool IsInvincible() const { return bIsInvincible; }

	void SetInvincible(bool NewIsInvincible) { bIsInvincible = NewIsInvincible};

private:
	UFUNCTION(NetMulticast, Reliable)
	void OnCurrentHPChanged_NetMulticast(float InOldCurrentHP, float InNewCurrentHP);

public:
	FOnOutOfCurrentHPDelegate OnOutOfCurrentHPDelegate;

	FOnCurrentHPChangeDelegate OnCurrentHPChangedDelegate;

	FOnMaxHPChangeDelegate OnMaxHPChangedDelegate;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UGStatComponent", meta = (AllowPrivateAccess))
	TObjectPtr<class UGGameInstance> GameInstance;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "UGStatComponent", meta = (AllowPrivateAccess))
	float MaxHP;

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "UGStatComponent", meta = (AllowPrivateAccess))
	float CurrentHP;

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "UGStatComponent", meta = (AllowPrivateAccess))
	uint8 bIsInvincible;
	
};
