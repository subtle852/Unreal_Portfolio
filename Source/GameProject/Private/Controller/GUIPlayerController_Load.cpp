// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/GUIPlayerController_Load.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"

void AGUIPlayerController_Load::BeginPlay()
{
	Super::BeginPlay();

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	if (IsValid(GameMode) == true)
	{
		FString NextLevelString = UGameplayStatics::ParseOption(GameMode->OptionsString, FString(TEXT("NextLevel")));
		UGameplayStatics::OpenLevel(GameMode, *NextLevelString, false);
	}
}

