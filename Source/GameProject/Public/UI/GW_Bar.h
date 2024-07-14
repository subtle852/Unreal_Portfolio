// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GWidget.h"
#include "GW_Bar.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UGW_Bar : public UGWidget
{
	GENERATED_BODY()

public:
	UGW_Bar(const FObjectInitializer& ObjectInitializer); 
	// Widget 클래스 생성자에서 로직을 작성해야한다면
	// 반드시 해당 생성자를 선언

	void SetMaxFigure(float InMaxFigure);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGW_Bar")
	TObjectPtr<UProgressBar> Bar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGW_Bar")
	float MaxFigure;
	
};

