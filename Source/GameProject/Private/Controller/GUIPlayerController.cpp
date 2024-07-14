// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/GUIPlayerController.h"
#include "Blueprint/UserWidget.h"

void AGUIPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (true == ::IsValid(UIWidgetClass))
    {
        UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass); 
        // CreateWidget()이 호출될 때 UIWidgetInstance->NativeOnInitialize() 함수가 호출
        if (true == ::IsValid(UIWidgetInstance))
        {
            UIWidgetInstance->AddToViewport();                             
            // AddToViewport()가 호출 될 때 UIWidgetInstance->NativeConstruct() 함수가 호출

            FInputModeUIOnly Mode;
            Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
            SetInputMode(Mode);

            bShowMouseCursor = true;
        }
    }
}
