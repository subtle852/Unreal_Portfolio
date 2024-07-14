// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_InGameESCMenu.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UUW_InGameESCMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnResumeButtonClicked();

	UFUNCTION()
	void OnReturnTitleButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UGMenuWidget, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> ResumeButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UGMenuWidget, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> ReturnTitleButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UGMenuWidget, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> ExitButton;
};
