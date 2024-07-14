// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GHUD.h"
#include "Component/GStatComponent.h"
#include "Game/GPlayerState.h"
#include "UI/GW_HPBar.h"
#include "Components/TextBlock.h"
#include "Game/GGameInstance.h"

void UGHUD::BindStatComponent(UGStatComponent* InStatComponent)
{
	if (IsValid(InStatComponent) == true)
	{
		StatComponent = InStatComponent;
		StatComponent->OnCurrentHPChangedDelegate.AddDynamic(HPBar, &UGW_HPBar::OnCurrentHPChange);
		StatComponent->OnMaxHPChangedDelegate.AddDynamic(HPBar, &UGW_HPBar::OnMaxHPChange);

		// PlayerController::BeginPlay()가
		// GStatComponent::BeginPlay()보다 먼저 호출되기에 아래와 같이 초기화
		UGGameInstance* GameInstance = Cast<UGGameInstance>(GetWorld()->GetGameInstance());
		if (IsValid(GameInstance) == true)
		{
			if (GameInstance->GetCharacterStatDataTable() != nullptr || GameInstance->GetCharacterStatDataTableRow(1) != nullptr)
			{
				float MaxHP = GameInstance->GetCharacterStatDataTableRow(1)->MaxHP;
				HPBar->SetMaxHP(MaxHP);
				HPBar->InitializeHPBarWidget(StatComponent.Get());
			}
		}
	}
}

void UGHUD::BindPlayerState(AGPlayerState* InPlayerState)
{
	if (IsValid(InPlayerState) == true)
	{
		PlayerState = InPlayerState;
		PlayerState->OnCurrentKillCountChangedDelegate.AddDynamic(this, &ThisClass::OnKillCountChanged);

		PlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName()));
		OnKillCountChanged(0, PlayerState->GetCurrentKillCount());
	}
}

void UGHUD::OnKillCountChanged(int32 InOldKillCount, int32 InNewKillCount)
{
	FString KillCountString = FString::Printf(TEXT("%d"), InNewKillCount);
	KillCountText->SetText(FText::FromString(KillCountString));
}

