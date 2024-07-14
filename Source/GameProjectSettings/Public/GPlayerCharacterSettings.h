#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GPlayerCharacterSettings.generated.h"

/**
 * 
 */
// 언리얼 엔진의 초기화 단계에서 
// Config 폴더에 위치한 DefaultPlayerCharacterMeshPaths.ini 파일을 읽어들이게 하는 키워드
UCLASS(config = PlayerCharacterMeshMaterialPaths)
class GAMEPROJECTSETTINGS_API UGPlayerCharacterSettings : public UObject
{
	GENERATED_BODY()
    
public:

	// 읽어들인 PlayerCharacterMeshMaterialPaths.ini 파일의 내용으로 
    // 해당 멤버의 기본값이 초기화
	UPROPERTY(config)
	TArray<FSoftObjectPath> PlayerCharacterMeshMaterialPaths;
};
