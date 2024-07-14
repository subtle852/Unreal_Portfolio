// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UGWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UGWidgetComponent();
	
	virtual void InitWidget() override;
	
};

