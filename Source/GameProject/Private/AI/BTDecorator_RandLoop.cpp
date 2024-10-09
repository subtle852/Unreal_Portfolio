// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_RandLoop.h"

#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/Composites/BTComposite_SimpleParallel.h"
#include "BehaviorTree/Decorators/BTDecorator_Loop.h"
 
#include UE_INLINE_GENERATED_CPP_BY_NAME(BTDecorator_RandLoop)

UBTDecorator_RandLoop::UBTDecorator_RandLoop(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "RandomLoop";
	MinLoopsNum = 0;
	MaxLoopsNum = 2;
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
 
	bAllowAbortNone = false;
	bAllowAbortLowerPri = false;
	bAllowAbortChildNodes = false;
}
 
void UBTDecorator_RandLoop::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	FBTLoopDecoratorMemory* DecoratorMemory = GetNodeMemory<FBTLoopDecoratorMemory>(SearchData);
	FBTCompositeMemory* ParentMemory = GetParentNode()->GetNodeMemory<FBTCompositeMemory>(SearchData);
	const bool bIsSpecialNode = GetParentNode()->IsA(UBTComposite_SimpleParallel::StaticClass());
 
	if ((bIsSpecialNode && ParentMemory->CurrentChild == BTSpecialChild::NotInitialized) ||
		(!bIsSpecialNode && ParentMemory->CurrentChild != ChildIndex))
	{
		int32 numLoops = FMath::RandRange(MinLoopsNum, MaxLoopsNum);
		DecoratorMemory->RemainingExecutions = IntCastChecked<uint8>(numLoops);
		DecoratorMemory->TimeStarted = GetWorld()->GetTimeSeconds();
	}
 
	bool bShouldLoop = false;
	if (DecoratorMemory->RemainingExecutions > 0)
	{
		DecoratorMemory->RemainingExecutions--;
	}
	bShouldLoop = DecoratorMemory->RemainingExecutions > 0;
	
	if (bShouldLoop)
	{
		GetParentNode()->SetChildOverride(SearchData, ChildIndex);
	}
}
 
FString UBTDecorator_RandLoop::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %d-%d loops"), *Super::GetStaticDescription(), MinLoopsNum, MaxLoopsNum);
}
 
void UBTDecorator_RandLoop::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
 
	FBTLoopDecoratorMemory* DecoratorMemory = (FBTLoopDecoratorMemory*)NodeMemory;
	Values.Add(FString::Printf(TEXT("loops remaining: %d"), DecoratorMemory->RemainingExecutions));
}
 
uint16 UBTDecorator_RandLoop::GetInstanceMemorySize() const
{
	return sizeof(FBTLoopDecoratorMemory);
}
 
void UBTDecorator_RandLoop::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FBTLoopDecoratorMemory>(NodeMemory, InitType);
}
 
void UBTDecorator_RandLoop::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTLoopDecoratorMemory>(NodeMemory, CleanupType);
}
 
#if WITH_EDITOR
 
FName UBTDecorator_RandLoop::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Decorator.Loop.Icon");
}
 
#endif	// WITH_EDITOR