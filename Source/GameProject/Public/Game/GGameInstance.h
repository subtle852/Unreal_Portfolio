#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "GGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FSStatTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FSStatTableRow()
	{
		
	}

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxKillCount;
	
};

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	virtual void Shutdown() override;

	const UDataTable* GetCharacterStatDataTable() const { return CharacterStatDataTable; }

	FSStatTableRow* GetCharacterStatDataTableRow(int32 InLevel) const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UDataTable> CharacterStatDataTable;
	
};
