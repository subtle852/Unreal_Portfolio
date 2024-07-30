// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GStatComponent.h"
#include "Game/GGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UGStatComponent::UGStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = false;
}

void UGStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MaxHP);
	DOREPLIFETIME(ThisClass, CurrentHP);
}

// Called when the game starts
void UGStatComponent::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UGGameInstance>(GetWorld()->GetGameInstance());
	if (::IsValid(GameInstance) == true)
	{
		if (GameInstance->GetCharacterStatDataTable() != nullptr
			|| GameInstance->GetCharacterStatDataTableRow(1) != nullptr)
		{
			float NewMaxHP = GameInstance->GetCharacterStatDataTableRow(1)->MaxHP;
			SetMaxHP(NewMaxHP);
			SetCurrentHP(NewMaxHP);
		}
	}
}

// Called every frame
void UGStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UGStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
}

void UGStatComponent::SetMaxHP(float InMaxHP)
{
	if (OnMaxHPChangedDelegate.IsBound() == true)
	{
		OnMaxHPChangedDelegate.Broadcast(MaxHP, InMaxHP);
	}

	MaxHP = FMath::Clamp<float>(InMaxHP, 0.f, 9999);
}

void UGStatComponent::SetCurrentHP(float InCurrentHP)
{
	if (OnCurrentHPChangedDelegate.IsBound() == true)
	{
		OnCurrentHPChangedDelegate.Broadcast(CurrentHP, InCurrentHP);
	}

	CurrentHP = FMath::Clamp<float>(InCurrentHP, 0.f, MaxHP);

	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		OnOutOfCurrentHPDelegate.Broadcast();
		CurrentHP = 0.f;
	}

	OnCurrentHPChanged_NetMulticast(CurrentHP, CurrentHP);
}

void UGStatComponent::OnCurrentHPChanged_NetMulticast_Implementation(float InOldCurrentHP, float InNewCurrentHP)
{
	if (OnCurrentHPChangedDelegate.IsBound() == true)
	{
		OnCurrentHPChangedDelegate.Broadcast(InOldCurrentHP, InNewCurrentHP);
	}

	if (InNewCurrentHP < KINDA_SMALL_NUMBER)
	{
		OnOutOfCurrentHPDelegate.Broadcast();
	}
}

