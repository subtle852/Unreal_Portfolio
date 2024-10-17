// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_DetectPlayerForHUD.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GMonster.h"
#include "Character/GPlayerCharacter.h"
#include "Component/GStatComponent.h"
#include "Controller/GAIController.h"
#include "Controller/GPlayerController.h"
#include "Engine/OverlapResult.h"
#include "Kismet/KismetSystemLibrary.h"

class AGPlayerCharacter;

UBTService_DetectPlayerForHUD::UBTService_DetectPlayerForHUD(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("DetectPlayerForHUD");
	Interval = 1.f;
	DetectRadius = 800.f;
}

void UBTService_DetectPlayerForHUD::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AGAIController* AIC = Cast<AGAIController>(OwnerComp.GetAIOwner());
		if (IsValid(AIC) == false)
			return;

		AGMonster* Monster = Cast<AGMonster>(AIC->GetPawn());
		if (IsValid(Monster) == false)
			return;

		UWorld* World = Monster->GetWorld();
		if (IsValid(World) == false)
			return;

		FVector CenterPosition = Monster->GetActorLocation();
		//DetectRadius = 300.f;
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
				AGPlayerCharacter* PC = Cast<AGPlayerCharacter>(OverlapResult.GetActor());
				if (IsValid(PC) == true)
				{
					if (PC->GetController()->IsPlayerController() == true)
					{
						if(PC->GetStatComponent()->GetCurrentHP() >= KINDA_SMALL_NUMBER)
						{
							AGPlayerController* TargetController = Cast<AGPlayerController>(PC->GetController());
							
							TargetController->CreateAndDisplayBossHPBar(Monster);
							Monster->GetStatComponent()->SetCurrentHP(Monster->GetStatComponent()->GetCurrentHP());
							Monster->GetStatComponent()->SetMaxHP(Monster->GetStatComponent()->GetMaxHP());
							
							//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Detected!")));
							//DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Red, false, 0.5f);
							//DrawDebugPoint(World, PC->GetActorLocation(), 10.f, FColor::Red, false, 0.5f);
							//DrawDebugLine(World, Monster->GetActorLocation(), PC->GetActorLocation(), FColor::Red, false, 0.5f, 0u, 3.f);

							//Monster->DrawDetectLine(true, CenterPosition, DetectRadius, PC->GetActorLocation(), Monster->GetActorLocation());
							return;
						}
					}
				}
			}
		}
		//Monster->DrawDetectLine(false, CenterPosition, DetectRadius, FVector::ZeroVector, Monster->GetActorLocation());
		//DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Green, false, 0.5f);
}
