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

protected:
	virtual void BeginPlay() override;
	
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
	
};
