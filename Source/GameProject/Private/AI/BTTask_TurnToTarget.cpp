// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TurnToTarget.h"
#include "Controller/GAIController.h"
#include "Character/GMonster.h"
#include "Character/GCharacter.h"
#include "Character/GPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
    NodeName = TEXT("TurnToTargetActor");

    TurnToTargetInterpSpeed = 5.f;
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    AGAIController* AIController = Cast<AGAIController>(OwnerComp.GetAIOwner());
    ensureMsgf(IsValid(AIController), TEXT("Invalid AIController"));

    AGMonster* Monster = Cast<AGMonster>(AIController->GetPawn());
    checkf(IsValid(Monster) == true, TEXT("Invalid Monster"));
	
    if (AGPlayerCharacter* TargetPC = Cast<AGPlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AIController->TargetActorKey)))
    {
        FVector LookVector = TargetPC->GetActorLocation() - Monster->GetActorLocation();
        LookVector.Z = 0.f;
        FRotator TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
        Monster->SetActorRotation(FMath::RInterpTo(Monster->GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), TurnToTargetInterpSpeed));

        return Result = EBTNodeResult::Succeeded;
    }

    return Result = EBTNodeResult::Failed;
}

