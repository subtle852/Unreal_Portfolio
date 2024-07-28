// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/GPlayerController.h"
#include "UI/GHUD.h"
#include "Game/GPlayerState.h"
#include "Component/GStatComponent.h"
#include "Character/GCharacter.h"

void AGPlayerController::ToggleInGameESCMenu()
{
	ensureMsgf(IsValid(InGameESCMenuInstance) == true, TEXT("Invalid InGameESCMenuInstance"));

	if (bIsInGameESCMenuOn == false)
	{
		InGameESCMenuInstance->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(InGameESCMenuInstance->GetCachedWidget());
		SetInputMode(Mode);

		SetPause(true);// 게임 일시정지
		// 반드시 InputAction 에셋의 TriggerWhenPaused 속성을 true로 지정해야
		// Pause 상태에서도 해당 입력 액션이 동작
		
		bShowMouseCursor = true;

		bIsInGameESCMenuOn = true;
	}
	else
	{
		InGameESCMenuInstance->SetVisibility(ESlateVisibility::Collapsed);

		FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);

		SetPause(false);// 게임 일시정지 해제
		// 반드시 InputAction 에셋의 TriggerWhenPaused 속성을 true로 지정해야
		// Pause 상태에서도 해당 입력 액션이 동작
		
		bShowMouseCursor = false;

		bIsInGameESCMenuOn = false;
	}
}

void AGPlayerController::ToggleCrossHair()
{
	if(bIsCrosshairUIOn == false)
	{
		CrosshairUIInstance->SetVisibility(ESlateVisibility::Visible);
		bIsCrosshairUIOn = true;
	}
	else
	{
		CrosshairUIInstance->SetVisibility(ESlateVisibility::Visible);
		bIsCrosshairUIOn = false;
	}
}

void AGPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// 마우스 클릭없이 바로 포커싱 되도록 하는 부분
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);

	if(HasAuthority() == true)
		return;

	ensureMsgf(IsValid(HUDWidgetClass), TEXT("Invalid HUDWidgetClass"));
	HUDWidget = CreateWidget<UGHUD>(this, HUDWidgetClass);
	if (IsValid(HUDWidget) == true)
	{
		HUDWidget->AddToViewport();

		AGPlayerState* GPlayerState = GetPlayerState<AGPlayerState>();
		if (IsValid(GPlayerState) == true)
		{
			HUDWidget->BindPlayerState(GPlayerState);
		}

		AGCharacter* PC = GetPawn<AGCharacter>();
		if (IsValid(PC) == true)
		{
			UGStatComponent* StatComponent = PC->GetStatComponent();
			if (IsValid(StatComponent) == true)
			{
				HUDWidget->BindStatComponent(StatComponent);
			}
		}
	}

	ensureMsgf(IsValid(InGameESCMenuClass), TEXT("Invalid InGameESCMenuClass"));
	InGameESCMenuInstance = CreateWidget<UUserWidget>(this, InGameESCMenuClass);
	if (IsValid(InGameESCMenuInstance) == true)
	{
		InGameESCMenuInstance->AddToViewport(3);

		InGameESCMenuInstance->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IsValid(CrosshairUIClass) == true)
	{
		CrosshairUIInstance = CreateWidget<UUserWidget>(this, CrosshairUIClass);
		if (IsValid(CrosshairUIInstance) == true)
		{
			CrosshairUIInstance->AddToViewport(1);

			CrosshairUIInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
