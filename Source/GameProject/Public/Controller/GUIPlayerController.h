// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GUIPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API AGUIPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	public:
    	virtual void BeginPlay() override;
    
    private:
    	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AGUIPlayerController, Meta = (AllowPrivateAccess))
    	TSubclassOf<UUserWidget> UIWidgetClass;
    
    	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AGUIPlayerController, Meta = (AllowPrivateAccess))
    	TObjectPtr<UUserWidget> UIWidgetInstance;
    	
};
