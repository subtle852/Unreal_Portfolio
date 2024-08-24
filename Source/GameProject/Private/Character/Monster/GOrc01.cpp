// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/GOrc01.h"

#include "NavigationSystem.h"
#include "AI/BTTask_Attack.h"
#include "Controller/GAIController.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/GAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnitConversion.h"

AGOrc01::AGOrc01()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_NavWalking;

	ClothMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ClothMeshComponent"));
	ClothMeshComponent->SetupAttachment(GetMesh());
	BeardMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BeardMeshComponent"));
	BeardMeshComponent->SetupAttachment(GetMesh());
	Armor01MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor01MeshComponent"));
	Armor01MeshComponent->SetupAttachment(GetMesh());
	Armor02MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor02MeshComponent"));
	Armor02MeshComponent->SetupAttachment(GetMesh());
	Armor03MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor03MeshComponent"));
	Armor03MeshComponent->SetupAttachment(GetMesh());
	Armor04MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor04MeshComponent"));
	Armor04MeshComponent->SetupAttachment(GetMesh());
	Armor05MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor05MeshComponent"));
	Armor05MeshComponent->SetupAttachment(GetMesh());
	Armor06MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor06MeshComponent"));
	Armor06MeshComponent->SetupAttachment(GetMesh());
	Armor07MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor07MeshComponent"));
	Armor07MeshComponent->SetupAttachment(GetMesh());
	Armor08MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor08MeshComponent"));
	Armor08MeshComponent->SetupAttachment(GetMesh());
	Armor09MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor09MeshComponent"));
	Armor09MeshComponent->SetupAttachment(GetMesh());
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
	WeaponMeshComponent->SetupAttachment(GetMesh());

	bIsNowAttacking = false;
	bIsNowMovingToBackFromTarget = false;
}

void AGOrc01::BeginPlay()
{
	Super::BeginPlay();

	ClothMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	BeardMeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	Armor01MeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	Armor02MeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	Armor03MeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	Armor04MeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	Armor05MeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	Armor06MeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	Armor07MeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	Armor08MeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);
	Armor09MeshComponent->SetLeaderPoseComponent(GetMesh(), true, false);

	FName WeaponSocket(TEXT("Azrak_Weapon_Soket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket) == true)
	{
		WeaponMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
	}

	//if (false == IsPlayerControlled())
	{
		//bUseControllerRotationYaw = false;
		bUseControllerRotationYaw = true;
		
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 500.f);
		GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);

		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
	
	ensureMsgf(IsValid(Attack01Montage), TEXT("Invalid Attack01Montage"));
	ensureMsgf(IsValid(Attack02Montage), TEXT("Invalid Attack02Montage"));
	ensureMsgf(IsValid(Attack03Montage), TEXT("Invalid Attack03Montage"));
}

void AGOrc01::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ensureMsgf(BlackboardDataAsset != nullptr, TEXT("Invalid BlackboardDataAsset"));
	ensureMsgf(BehaviorTree != nullptr, TEXT("Invalid BehaviorTree"));

	if (IsValid(NewController))
	{
		AGAIController* AIController = Cast<AGAIController>(NewController);
		AIController->InitializeAI(BlackboardDataAsset, BehaviorTree);
	}
}

void AGOrc01::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsNowMovingToBackFromTarget == true)
	{
		// if(FVector::Dist(GetActorLocation(), InitialLocationOfMovingToBackFromTarget) > 300.f)
		// {
		// 	bIsNowMovingToBackFromTarget = false;
		// }
	}
}

void AGOrc01::DrawDetectLine(const bool bResult, FVector CenterPosition, float DetectRadius, FVector PCLocation,
	FVector MonsterLocation)
{
	Super::DrawDetectLine(bResult, CenterPosition, DetectRadius, PCLocation, MonsterLocation);

	DrawDetectLine_NetMulticast(bResult, CenterPosition, DetectRadius, PCLocation, MonsterLocation);
}

void AGOrc01::DrawDetectLine_NetMulticast_Implementation(const bool bResult, FVector CenterPosition, float DetectRadius,
                                          FVector PCLocation, FVector MonsterLocation)
{
	if(HasAuthority() == true)
		return;

	if(bResult == false)
	{
		DrawDebugSphere(GetWorld(), CenterPosition, DetectRadius, 16, FColor::Green, false, 0.5f);
	}
	else
	{
		DrawDebugSphere(GetWorld(), CenterPosition, DetectRadius, 16, FColor::Red, false, 0.5f);
		DrawDebugPoint(GetWorld(), PCLocation, 10.f, FColor::Red, false, 0.5f);
		DrawDebugLine(GetWorld(), MonsterLocation, PCLocation, FColor::Red, false, 0.5f, 0u, 3.f);
	}
}

void AGOrc01::OnCheckHit()
{
	Super::OnCheckHit();

	UKismetSystemLibrary::PrintString(this, TEXT("OnCheckHit is called"));
	
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + BasicAttackRange * GetActorForwardVector(),
		FQuat::Identity,
		ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(BasicAttackRadius),
		Params
	);

	// DrawLine은 NetMulticast로
	DrawLine_NetMulticast(bResult);

	// Server
	if (bResult)
	{
		for (const FHitResult& HitResult : HitResults)	
		{
			if (::IsValid(HitResult.GetActor()))
			{
				UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));
				
				FDamageEvent DamageEvent;
				HitResult.GetActor()->TakeDamage(10.f, DamageEvent, GetController(), this);
			}
		}
	}
}

void AGOrc01::BeginAttack()
{
	Super::BeginAttack();

	UKismetSystemLibrary::PrintString(this, TEXT("BeginAttack is called"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	bIsNowAttacking = true;

	PlayBasicAttackAnimMontage_NetMulticast();
}

void AGOrc01::PlayBasicAttackAnimMontage_NetMulticast_Implementation()
{
	UKismetSystemLibrary::PrintString(this, TEXT("PlayBasicAttackAnimMontage is called by NetMulticast"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	int AttackRandNum = FMath::RandRange(01, 03);
	TObjectPtr<class UAnimMontage> AttackRandMontage = nullptr;
	
	switch (AttackRandNum)
	{
	case 1:
		AttackRandMontage = Attack01Montage;
		break;

	case 2:
		AttackRandMontage = Attack02Montage;
		break;

	case 3:
		AttackRandMontage = Attack03Montage;
		break;

	default:
		ensureMsgf(IsValid(AttackRandMontage), TEXT("Invalid AttackRandMontage"));
		break;
	}
	
	AnimInstance->PlayAnimMontage(AttackRandMontage);

	if (OnBasicAttackMontageEndedDelegate.IsBound() == false)
	{
		OnBasicAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnBasicAttackMontageEndedDelegate, AttackRandMontage);
	}
	AnimInstance->Montage_SetEndDelegate(OnBasicAttackMontageEndedDelegate_Task, AttackRandMontage);
}

void AGOrc01::DrawLine_NetMulticast_Implementation(const bool bResult)
{
	FVector TraceVector = BasicAttackRange * GetActorForwardVector();
	FVector Center = GetActorLocation() + TraceVector + GetActorUpVector() * 40.f;
	float HalfHeight = BasicAttackRange * 0.5f + BasicAttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.f;

	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		BasicAttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);
}

void AGOrc01::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	Super::EndAttack(InMontage, bInterruped);

	UKismetSystemLibrary::PrintString(this, TEXT("EndAttack is called"));
	
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bIsNowAttacking = false;

	if (OnBasicAttackMontageEndedDelegate.IsBound() == true)
	{
		OnBasicAttackMontageEndedDelegate.Unbind();
	}
}

void AGOrc01::MoveToBackFromTarget(const FVector& InDirection)
{
	Super::MoveToBackFromTarget(InDirection);

	InitialLocationOfMovingToBackFromTarget = GetActorLocation();
	bIsNowMovingToBackFromTarget = true;
	
	//AGAIController* AIController = Cast<AGAIController>(GetController());
	//AIController->MoveToLocation(InLocation);
	
	// AGAIController* AIController = Cast<AGAIController>(GetController());
	// AGCharacter* TargetActor = Cast<AGCharacter>(AIController->GetBlackboardComponent()->GetValueAsObject(AGAIController::TargetActorKey));
	//
	// FVector T = GetActorLocation();
	// FVector M = InLocation;
	// if(bIsNowMovingToBackFromTarget == true)
	// {
	// 	FVector temp = T;
	// }
	const FRotator ControlRotationTemp = GetController()->GetControlRotation();
	const FRotator ControlRotationYaw(0.f, ControlRotationTemp.Yaw, 0.f);
	
	const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(ForwardVector, InDirection.X);
	AddMovementInput(RightVector, InDirection.Y);
	
	
	//BeginMoveToBackFromTarget_Server(InLocation);
}

void AGOrc01::BeginMoveToBackFromTarget_Server_Implementation(const FVector& InLocation)
 {
 	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
 	if (NavSystem == nullptr)
 	{
 		AGAIController* AIController = Cast<AGAIController>(GetController());
 		AIController->MoveToLocation(InLocation);
 	}
 }

void AGOrc01::BeginShout()
{
	Super::BeginShout();
	
	UKismetSystemLibrary::PrintString(this, TEXT("BeginShout is called"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	bIsShout = true;

	PlayShoutAnimMontage_NetMulticast();
}

void AGOrc01::PlayShoutAnimMontage_NetMulticast_Implementation()
{
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	AnimInstance->PlayAnimMontage(ShoutMontage);

	if (OnShoutMontageEndedDelegate.IsBound() == false)
	{
		OnShoutMontageEndedDelegate.BindUObject(this, &ThisClass::EndShout);
		AnimInstance->Montage_SetEndDelegate(OnShoutMontageEndedDelegate, ShoutMontage);
	}
	AnimInstance->Montage_SetEndDelegate(OnShoutMontageEndedDelegate_Task, ShoutMontage);
	
}

void AGOrc01::EndShout(UAnimMontage* InMontage, bool bInterruped)
{
	Super::EndShout(InMontage, bInterruped);	

	bIsShout = true;

	if (OnShoutMontageEndedDelegate.IsBound() == true)
	{
		OnShoutMontageEndedDelegate.Unbind();
	}
}


