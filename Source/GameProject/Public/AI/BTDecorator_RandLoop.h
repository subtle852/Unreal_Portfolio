// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_RandLoop.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UBTDecorator_RandLoop : public UBTDecorator
{
	GENERATED_UCLASS_BODY()
	
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
	virtual FString GetStaticDescription() const override;
 
#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR
 
protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	
	UPROPERTY(EditAnywhere, Category= "Decorator" ,meta=(ClampMin="0", ClampMax="255"))
	int32 MinLoopsNum;
 
	UPROPERTY(EditAnywhere, Category= "Decorator" ,meta=(ClampMin="0", ClampMax="255"))
	int32 MaxLoopsNum;
	
};
