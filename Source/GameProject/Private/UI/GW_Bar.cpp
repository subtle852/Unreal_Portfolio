// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GW_Bar.h"
#include "Components/ProgressBar.h"

UGW_Bar::UGW_Bar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGW_Bar::SetMaxFigure(float InMaxFigure)
{
	if (InMaxFigure < KINDA_SMALL_NUMBER)
	{
		MaxFigure = 0.f;
	}
	else
	{
		MaxFigure = InMaxFigure;
	}
}

void UGW_Bar::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 하드코딩으로 위젯과 속성을 바인드하는 방법으로
	// 추후에는 다른 방식으로 사용 예정
	Bar = Cast<UProgressBar>(GetWidgetFromName("BarWidget"));
	ensureMsgf(IsValid(Bar), TEXT("Invalid Bar"));
	
}

