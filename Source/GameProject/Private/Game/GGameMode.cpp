#include "Game/GGameMode.h"
#include "Controller/GPlayerController.h"

AGGameMode::AGGameMode()
{
	PlayerControllerClass = AGPlayerController::StaticClass();
}
