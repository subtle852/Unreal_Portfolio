// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GMonster.h"
#include "Character/GPlayerCharacter.h"
#include "Controller/GAIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/GStatComponent.h"
#include "UI/GW_HPBar.h"
#include "Component/GWidgetComponent.h"
#include "Game/GPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AGMonster::AGMonster()
{
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.TickInterval = 0.1f;
	bReplicates = true;

	AIControllerClass = AGAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// AGMonster는 레벨에 배치되거나 새롭게 생성되면 GAIController의 빙의가 자동으로 진행됨.

	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	bIsNowAttacking = false;
	bIsNowMovingToBackFromTarget = false;
	bIsNowHovering = false;
	bIsShout = false;
	bIsNowTeleporting = false;

	WidgetComponent = CreateDefaultSubobject<UGWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetRootComponent());
	WidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	// WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// Billboard 방식으로 보이지만, 주인공 캐릭터를 가리게된다
	// 또한 UI와 멀어져도 동일한 크기가 유지된다
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void AGMonster::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HasAuthority()==true)
	{
		//UKismetSystemLibrary::PrintString(this, TEXT("Monster Tick is called in Server"));
	}
	else
	{
		if (GetOwner() == UGameplayStatics::GetPlayerController(this, 0))
		{
			//UKismetSystemLibrary::PrintString(this, TEXT("Monster Tick is called in Owning"));
			// AIController이기에 해당 부분은 호출되지 않음
		}
		else
		{
			//UKismetSystemLibrary::PrintString(this, TEXT("Monster Tick is called in Other"));

			if (IsValid(WidgetComponent) == true)
			{
				//UKismetSystemLibrary::PrintString(this, TEXT("Monster Widget is being Rotated"));
					
				FVector WidgetComponentLocation = WidgetComponent->GetComponentLocation();
				FVector LocalPlayerCameraLocation = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
				WidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(WidgetComponentLocation, LocalPlayerCameraLocation));
			}
		}
	}
}

void AGMonster::SetWidget(UGWidget* InGWidget)
{
	Super::SetWidget(InGWidget);
	
	UGW_HPBar* HPBarWidget = Cast<UGW_HPBar>(InGWidget);
	if (IsValid(HPBarWidget) == true)
	{
		HPBarWidget->SetMaxHP(StatComponent->GetMaxHP());
		HPBarWidget->InitializeHPBarWidget(StatComponent);
		StatComponent->OnCurrentHPChangedDelegate.AddDynamic(HPBarWidget, &UGW_HPBar::OnCurrentHPChange);
	}
}

float AGMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                            AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Super::TakeDamage()에서 죽음관련 처리되고 있음
	// 여기서는 AI만 종료
	//if (CurrentHP < KINDA_SMALL_NUMBER)
	if (GetStatComponent()->GetCurrentHP() < KINDA_SMALL_NUMBER)
	{
		AGAIController* AIController = Cast<AGAIController>(GetController());
		if (true == ::IsValid(AIController))
		{
			AIController->EndAI();
		}

		AGPlayerCharacter* DamageCauserCharacter = Cast<AGPlayerCharacter>(DamageCauser);
		if (true == ::IsValid(DamageCauserCharacter))
		{
			//DamageCauserCharacter->AddCurrentKillCount(1);

			AGPlayerState* GPlayerState = Cast<AGPlayerState>(DamageCauserCharacter->GetPlayerState());
			if (IsValid(GPlayerState) == true)
			{
				GPlayerState->AddCurrentKillCount(1);
			}
		}
		
		Destroy();
	}
	
	return FinalDamageAmount;
}

void AGMonster::OnCheckHit()
{
}

void AGMonster::OnShootProjectile()
{
}

void AGMonster::OnShootWindProjectile()
{
}

void AGMonster::OnShootMultipleProjectile()
{
}

void AGMonster::OnShootShapeAOE()
{
}

void AGMonster::OnShootLaser()
{
}

void AGMonster::OnShootAOE()
{
}

void AGMonster::DrawDetectLine(const bool bResult, FVector CenterPosition, float DetectRadius, FVector PCLocation, FVector MonsterLocation)
{
}

void AGMonster::OnJump()
{
}

void AGMonster::BeginAttack()
{
}

void AGMonster::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
}

void AGMonster::BeginShoot()
{
}

void AGMonster::EndShoot(UAnimMontage* InMontage, bool bInterruped)
{
}

void AGMonster::BeginShootWind()
{
}

void AGMonster::EndShootWind(UAnimMontage* InMontage, bool bInterruped)
{
}

void AGMonster::BeginShootMultiple()
{
}

void AGMonster::EndShootMultiple(UAnimMontage* InMontage, bool bInterruped)
{
}

void AGMonster::BeginShootAOE()
{
}

void AGMonster::EndShootAOE(UAnimMontage* InMontage, bool bInterruped)
{
}

void AGMonster::BeginShootLaser()
{
}

void AGMonster::EndShootLaser(UAnimMontage* InMontage, bool bInterruped)
{
}

void AGMonster::Teleport()
{
}

void AGMonster::TeleportEnd()
{
}

void AGMonster::MoveToBackFromTarget(const FVector& InDirection)
{
}

void AGMonster::BeginShout()
{
}

void AGMonster::EndShout(UAnimMontage* InMontage, bool bInterruped)
{
}
