// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_DetectAnotherTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GMonster.h"
#include "Controller/GAIController.h"
#include "Engine/OverlapResult.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTask_DetectAnotherTarget::UBTTask_DetectAnotherTarget()
{
	NodeName = TEXT("DetectAnotherTarget");
	bNotifyTick = true;

	DetectRadius = 800.f;
	bIsDetectAnotherTarget = false;
}

void UBTTask_DetectAnotherTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UBTTask_DetectAnotherTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGAIController* AIC = Cast<AGAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIC) == false)
		return EBTNodeResult::Failed;

	AGMonster* Monster = Cast<AGMonster>(AIC->GetPawn());
	if (IsValid(Monster) == false)
		return EBTNodeResult::Failed;

	UWorld* World = Monster->GetWorld();
	if (IsValid(World) == false)
		return EBTNodeResult::Failed;

	FVector CenterPosition = Monster->GetActorLocation();
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParams(NAME_None, false, Monster);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		CenterPosition,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParams
	);

	if (bResult == true)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			AGCharacter* PC = Cast<AGCharacter>(OverlapResult.GetActor());
			if (IsValid(PC) == true)
			{
				if (PC->GetController()->IsPlayerController() == true)
				{
					if(AIC->TargetActor != PC)// 다른 타겟 액터 감지한 경우
					{
						OwnerComp.GetBlackboardComponent()->SetValueAsObject(AGAIController::TargetActorKey, PC);
						AIC->TargetActor = PC;

						bIsDetectAnotherTarget = true;
						
						break;
					}
				}
			}
		}
	}

	if(static_cast<bool>(bIsDetectAnotherTarget) == true)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Detect Another Target!")));

		Monster->DrawDetectLine(true, CenterPosition, DetectRadius, AIC->TargetActor->GetActorLocation(),
			Monster->GetActorLocation());
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Can't Detect Another Target!")));

		Monster->DrawDetectLine(false, CenterPosition, DetectRadius, FVector::ZeroVector,
			Monster->GetActorLocation());
	}
	
	return EBTNodeResult::Succeeded;
}
