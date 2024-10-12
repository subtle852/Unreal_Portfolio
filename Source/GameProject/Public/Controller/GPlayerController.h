// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GPlayerController.generated.h"

class UGHUD;

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API AGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UGHUD* GetHUDWidget() const { return HUDWidget; };

	void ToggleInGameESCMenu();
	
	void ToggleCrossHair(bool bInWantedToggleOn);

	UFUNCTION(Client, Reliable)
	void CreateAndDisplayBossHPBar(AGMonster* BossMonster);

protected:
	virtual void BeginPlay() override;

	void CheckAndBindPlayerState();
	
private:
	UPROPERTY();
	TObjectPtr<UGHUD> HUDWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AGPlayerController, meta = (AllowPrivateAccess));
	TSubclassOf<UGHUD> HUDWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AGPlayerController, meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> InGameESCMenuClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AGPlayerController, meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> InGameESCMenuInstance;

	bool bIsInGameESCMenuOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AGPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> CrosshairUIClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AGPlayerController, meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> CrosshairUIInstance;

	bool bIsCrosshairUIOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|UI", meta = (AllowPrivateAccess))
	TObjectPtr<class UGW_HPBar> BossHPBarWidgetInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGBoss01|UI", meta = (AllowPrivateAccess))
	TSubclassOf<class UGW_HPBar> BossHPBarWidgetClass;

	bool bIsBossHPBarWidgetOn = false;

	FTimerHandle CheckAndBindPlayerStateTimerHandle;
	
};

