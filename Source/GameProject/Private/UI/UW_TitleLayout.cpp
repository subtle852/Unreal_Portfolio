// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_TitleLayout.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

UUW_TitleLayout::UUW_TitleLayout(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UUW_TitleLayout::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UUW_TitleLayout::OnPlayButtonClicked()
{
	//UGameplayStatics::OpenLevel(GetWorld(), TEXT("DemoLevel"));

	// 로딩 레벨로 넘어가더라도 관련 정보가 다 지워지지 않도록 하기위한 부분
	// NextLevel이 Key, DemoLevel이 Value
	// Load 레벨에서는 NextLevel을 파싱해서 DemoLevel 값을 얻어냄
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("LoadLevel")), true, FString(TEXT("NextLevel=LobbyLevel")));

}

void UUW_TitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
