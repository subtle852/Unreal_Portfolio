// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UGWidget : public UUserWidget
{
	GENERATED_BODY()
	
	public:
        AActor* GetOwningActor() const { return OwningActor; }
    
        void SetOwningActor(AActor* InOwningActor) { OwningActor = InOwningActor; }
    
    protected:
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UGWidget)
        TObjectPtr<AActor> OwningActor;
	
};
