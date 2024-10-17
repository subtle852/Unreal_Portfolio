// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/GPlayerController.h"
#include "UI/GHUD.h"
#include "Game/GPlayerState.h"
#include "Component/GStatComponent.h"
#include "Character/GCharacter.h"
#include "Character/GMonster.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "UI/GW_HPBar.h"

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

void AGPlayerController::ToggleCrossHair(bool bInWantedToggleOn)
{
	if(bInWantedToggleOn == true)
	{
		CrosshairUIInstance->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CrosshairUIInstance->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AGPlayerController::CreateAndDisplayBossHPBar_Implementation(AGMonster* BossMonster)
{
	if (IsLocalController())
	{
		if(bIsBossHPBarWidgetOn == true)
			return;
			
		if (BossHPBarWidgetClass)// BossHPBarWidgetTemplate은 블루프린트로 설정된 위젯클래스
		{
			BossHPBarWidgetInstance = CreateWidget<UGW_HPBar>(this, BossHPBarWidgetClass);
			if (BossHPBarWidgetInstance)
			{
				// HUD에 추가
				UVerticalBox* TargetHUDTopVerticalBox = HUDWidget->GetTopVerticalBox();
				if (TargetHUDTopVerticalBox)
				{
					TargetHUDTopVerticalBox->AddChildToVerticalBox(BossHPBarWidgetInstance);
				}
				
				// 델리게이트 바인드
				BossMonster->GetStatComponent()->OnCurrentHPChangedDelegate.AddDynamic(BossHPBarWidgetInstance, &UGW_HPBar::OnCurrentHPChange);
				BossMonster->GetStatComponent()->OnMaxHPChangedDelegate.AddDynamic(BossHPBarWidgetInstance, &UGW_HPBar::OnMaxHPChange);

				// 현재 체력이 바로 적용 되도록 델리게이트 강제 호출
				// (사실 클라에서 단독으로 호출하는 거라 의미가 없음, 서버에서 호출하고 뿌려주는 방식으로 동작하는 것이 근본이기 때문) 
				BossMonster->GetStatComponent()->SetCurrentHP(BossMonster->GetStatComponent()->GetCurrentHP());
				BossMonster->GetStatComponent()->SetMaxHP(BossMonster->GetStatComponent()->GetMaxHP());
				
				// BGM
				if (BGMInstance->IsPlaying())
				{
					BGMInstance->Stop();
				}
				if (BGM_BOSS)
				{
					BGMInstance->SetSound(BGM_BOSS);
					BGMInstance->Play();
				}
				
				bIsBossHPBarWidgetOn = true;

				// 시도해봤던 첫번째 방식
				// BossHPBarWidgetInstance->SetMaxHP(100.f);
				// BossHPBarWidgetInstance->InitializeHPBarWidget(BossMonster->GetStatComponent());

				// 시도해봤던 두번째 방식
				// float CurrentHP = BossMonster->GetStatComponent()->GetCurrentHP();
				// float MaxHP = BossMonster->GetStatComponent()->GetMaxHP();
				// BossHPBarWidgetInstance->OnMaxHPChange(MaxHP, MaxHP);
				// BossHPBarWidgetInstance->OnCurrentHPChange(CurrentHP, MaxHP);
			}
		}
	}
}

AGPlayerController::AGPlayerController()
{
	BGMInstance = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMInstance"));
	BGMInstance->bAutoActivate = false;
}

void AGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority() == false)
	{
		// 마우스 클릭없이 바로 포커싱 되도록 하는 부분
		FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);

		ensureMsgf(IsValid(HUDWidgetClass), TEXT("Invalid HUDWidgetClass"));
		HUDWidget = CreateWidget<UGHUD>(this, HUDWidgetClass);
		if (IsValid(HUDWidget) == true)
		{
			HUDWidget->AddToViewport();

			GetWorld()->GetTimerManager().SetTimer(CheckAndBindPlayerStateTimerHandle, this, &AGPlayerController::CheckAndBindPlayerState, 1.0f, false);
			
			// AGPlayerState* GPlayerState = GetPlayerState<AGPlayerState>();
			// if (IsValid(GPlayerState) == true)
			// {
			// 	HUDWidget->BindPlayerState(GPlayerState);
			// }

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

	if (GEngine->GetNetMode(GetWorld()) != NM_Standalone)
	{
		if (IsValid(BGM_BASIC))
		{
			BGMInstance->SetSound(BGM_BASIC);
			BGMInstance->Play();
		}
	}
}

void AGPlayerController::CheckAndBindPlayerState()
{
	AGPlayerState* GPlayerState = GetPlayerState<AGPlayerState>();
	if (IsValid(GPlayerState) == true)
	{
		HUDWidget->BindPlayerState(GPlayerState);
	}
}
