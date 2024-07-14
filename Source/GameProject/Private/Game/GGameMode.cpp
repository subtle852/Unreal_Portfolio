#include "Game/GGameMode.h"
#include "Controller/GPlayerController.h"
#include "Game/GPlayerState.h"

AGGameMode::AGGameMode()
{
	PlayerControllerClass = AGPlayerController::StaticClass();
}

void AGGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AGPlayerState* PlayerState = NewPlayer->GetPlayerState<AGPlayerState>();
	if (::IsValid(PlayerState) == true)
	{
		PlayerState->InitPlayerState();
	}
	
}
