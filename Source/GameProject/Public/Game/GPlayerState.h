// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerTeam : uint8
{
	None,
	Black,
	White,
	End
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentKillCountChangedDelegate, int32, InOldCurrentKillCount, int32, InNewCurrentKillCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentWeaponChangedDelegate, int32, InWeaponNumber);

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API AGPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AGPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitPlayerState();
	
	EPlayerTeam GetPlayerTeam() const { return PlayerTeam; }

	int32 GetMaxKillCount() const { return MaxKillCount; }
	
	void SetMaxKillCount(int32 InMaxKillCount) { MaxKillCount = InMaxKillCount; }
	
	int32 GetCurrentKillCount() const { return CurrentKillCount; }
	
	void AddCurrentKillCount(int32 InCurrentKillCount);
	
	void SetWeaponType(int32 InWeaponType);
    
public:
	FOnCurrentKillCountChangedDelegate OnCurrentKillCountChangedDelegate;
	
	FOnCurrentWeaponChangedDelegate OnCurrentWeaponTypeChangedDelegate;
	
	UFUNCTION()
	void OnRep_CurrentKillCount()
	{
		OnCurrentKillCountChangedDelegate.Broadcast(CurrentKillCount, CurrentKillCount);
	}

private:
	UPROPERTY(Replicated = OnRep_CurrentKillCount, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	EPlayerTeam PlayerTeam = EPlayerTeam::None;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 MaxKillCount = 99;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentKillCount, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 CurrentKillCount = 0;
	
};

