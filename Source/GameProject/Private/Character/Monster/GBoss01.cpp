// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/GBoss01.h"

#include "NavigationSystem.h"
#include "AI/BTTask_Attack.h"
#include "Controller/GAIController.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/GAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Item/GWeaponActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnitConversion.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleSystemComponent.h"

AGBoss01::AGBoss01()
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

void AGBoss01::BeginPlay()
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

	
	FName WeaponSocket(TEXT("Weapon_Socket_R"));
	if (GetMesh()->DoesSocketExist(WeaponSocket) == true)
	{
		WeaponInstance = GetWorld()->SpawnActor<AGWeaponActor>(WeaponClass, FVector::ZeroVector,
															   FRotator::ZeroRotator);
		if (IsValid(WeaponInstance) == true)
		{
			WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
											  WeaponSocket);
		}
	}

	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void AGBoss01::PossessedBy(AController* NewController)
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

void AGBoss01::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGBoss01::DrawDetectLine(const bool bResult, FVector CenterPosition, float DetectRadius, FVector PCLocation,
	FVector MonsterLocation)
{
	Super::DrawDetectLine(bResult, CenterPosition, DetectRadius, PCLocation, MonsterLocation);

	DrawDetectLine_NetMulticast(bResult, CenterPosition, DetectRadius, PCLocation, MonsterLocation);
}

void AGBoss01::DrawDetectLine_NetMulticast_Implementation(const bool bResult, FVector CenterPosition,
	float DetectRadius, FVector PCLocation, FVector MonsterLocation)
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

void AGBoss01::OnCheckHit()
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

void AGBoss01::BeginAttack()
{
	Super::BeginAttack();
	
	UKismetSystemLibrary::PrintString(this, TEXT("BeginAttack is called"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	bIsNowAttacking = true;

	PlayBasicAttackAnimMontage_NetMulticast();
}

void AGBoss01::PlayBasicAttackAnimMontage_NetMulticast_Implementation()
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

void AGBoss01::DrawLine_NetMulticast_Implementation(const bool bResult)
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

void AGBoss01::EndAttack(UAnimMontage* InMontage, bool bInterruped)
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

void AGBoss01::Teleport()
{
	Super::Teleport();
	
	bIsNowTeleporting = true;
	
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->MaxFlySpeed = 2000.f;
	GetCharacterMovement()->MaxAcceleration = 99999.f;

	GetMesh()->SetVisibility(false, true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1,ECR_Ignore);

	
	Teleport_NetMulticast();
}

void AGBoss01::Teleport_NetMulticast_Implementation()
{
	if(HasAuthority() == true)
		return;
	
	bIsNowTeleporting = true;
	
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->MaxFlySpeed = 2000.f;
	GetCharacterMovement()->MaxAcceleration = 99999.f;

	GetMesh()->SetVisibility(false, true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1,ECR_Ignore);

	TeleportBodyEffectEmitterInstance = UGameplayStatics::SpawnEmitterAttached(
		TeleportBodyEffectEmitterTemplate,
		GetMesh(),
		FName("spine_01"),
		FVector(0.f, 0.f, 0.f),
		FRotator(0.f, 0.f, 0.f),
		EAttachLocation::KeepRelativeOffset,
		true,
		EPSCPoolMethod::None,
		true
	);
	
	TeleportTrailEffectEmitterInstance = UGameplayStatics::SpawnEmitterAttached(
		TeleportTrailEffectEmitterTemplate,
		GetMesh(),
		FName("spine_01"),
		FVector(0.f, 0.f, 0.f),
		FRotator(0.f, 0.f, 0.f),
		EAttachLocation::KeepRelativeOffset,
		true,
		EPSCPoolMethod::None,
		true
	);
}

void AGBoss01::TeleportEnd()
{
	Super::TeleportEnd();
	
	GetCharacterMovement()->StopMovementImmediately();
	
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->MaxAcceleration = 2048.f;

	GetMesh()->SetVisibility(true, true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1,ECR_Block);

	bIsNowTeleporting = false;
	
	TeleportEnd_NetMulticast();
}

void AGBoss01::TeleportEnd_NetMulticast_Implementation()
{
	if(HasAuthority() == true)
		return;
	
	GetCharacterMovement()->StopMovementImmediately();
	
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->MaxAcceleration = 2048.f;

	GetMesh()->SetVisibility(true, true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1,ECR_Block);

	bIsNowTeleporting = false;

	auto TeleportEndDelaylambda_Owner = [this]()
	{
		TeleportBodyEffectEmitterInstance->DestroyComponent();
		TeleportTrailEffectEmitterInstance->DestroyComponent();
	};
	
	FTimerDelegate TeleportEndDelayTimerDelegate;
	TeleportEndDelayTimerDelegate.BindLambda(TeleportEndDelaylambda_Owner);
	
	GetWorldTimerManager().SetTimer(TeleportEndDelayTimerHandle, TeleportEndDelayTimerDelegate, TeleportEndDelayThreshold, false);
	
}

void AGBoss01::MoveToBackFromTarget(const FVector& InDirection)
{
	Super::MoveToBackFromTarget(InDirection);

	//
}

void AGBoss01::BeginMoveToBackFromTarget_Server_Implementation(const FVector& InLocation)
{
	//
}

void AGBoss01::BeginShout()
{
	Super::BeginShout();
	
	UKismetSystemLibrary::PrintString(this, TEXT("BeginShout is called"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	bIsShout = true;

	PlayShoutAnimMontage_NetMulticast();
}

void AGBoss01::PlayShoutAnimMontage_NetMulticast_Implementation()
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

void AGBoss01::EndShout(UAnimMontage* InMontage, bool bInterruped)
{
	Super::EndShout(InMontage, bInterruped);
	
	bIsShout = true;

	if (OnShoutMontageEndedDelegate.IsBound() == true)
	{
		OnShoutMontageEndedDelegate.Unbind();
	}
}
