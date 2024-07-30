#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API AGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGGameMode();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
};

