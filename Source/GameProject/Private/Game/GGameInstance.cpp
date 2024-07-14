// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GGameInstance.h"

void UGGameInstance::Init()
{
    Super::Init();

    if (IsValid(CharacterStatDataTable) == false || CharacterStatDataTable->GetRowMap().Num() <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Not enuough data in CharacterStatDataTable."));
    }
    else
    {
        for (int32 i = 1; i <= CharacterStatDataTable->GetRowMap().Num(); ++i)
        {
            ensureMsgf(GetCharacterStatDataTableRow(i) != nullptr, TEXT("Invalid data in CharacterStatDataTable"));
        }
    }
}

void UGGameInstance::Shutdown()
{
    Super::Shutdown();
}

FSStatTableRow* UGGameInstance::GetCharacterStatDataTableRow(int32 InLevel) const
{
    if (IsValid(CharacterStatDataTable) == true)
    {
        return CharacterStatDataTable->FindRow<FSStatTableRow>(*FString::FromInt(InLevel), TEXT(""));
    }

    return nullptr;
}


