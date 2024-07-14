// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/GUIPlayerController.h"
#include "GUIPlayerController_Load.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API AGUIPlayerController_Load : public AGUIPlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
};

