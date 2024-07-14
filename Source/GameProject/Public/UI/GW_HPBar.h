// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GW_Bar.h"
#include "GW_HPBar.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UGW_HPBar : public UGW_Bar
{
	GENERATED_BODY()

public:
	void SetMaxHP(float InMaxHP);

	void InitializeHPBarWidget(class UGStatComponent* NewStatComponent);

	UFUNCTION()
	void OnMaxHPChange(float InOldMaxHP, float InNewMaxHP);

	UFUNCTION()
	void OnCurrentHPChange(float InOldHP, float InNewHP);

protected:
	virtual void NativeConstruct() override;
	
};

