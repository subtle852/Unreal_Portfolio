// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/GOrc01.h"
#include "Controller/GAIController.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/GAnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"

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

	if (false == IsPlayerControlled())
	{
		bUseControllerRotationYaw = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 250.f, 0.f);

		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
	
	ensureMsgf(IsValid(BasicAttackMontage), TEXT("Invalid MeleeAttackMontage"));
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

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	bIsNowAttacking = true;

	PlayBasicAttackAnimMontage_NetMulticast();
}

void AGOrc01::PlayBasicAttackAnimMontage_NetMulticast_Implementation()
{
	UKismetSystemLibrary::PrintString(this, TEXT("PlayBasicAttackAnimMontage is called by NetMulticast"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	AnimInstance->PlayAnimMontage(BasicAttackMontage);

	if (OnBasicAttackMontageEndedDelegate.IsBound() == false)
	{
		OnBasicAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnBasicAttackMontageEndedDelegate, BasicAttackMontage);
	}
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
	
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bIsNowAttacking = false;

	if (OnBasicAttackMontageEndedDelegate.IsBound() == true)
	{
		OnBasicAttackMontageEndedDelegate.Unbind();
	}
}

