// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/GBoss01.h"

#include "NavigationSystem.h"
#include "AI/BTTask_Attack.h"
#include "Controller/GAIController.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/GAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/GPlayerCharacter.h"
#include "Component/GStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/VerticalBox.h"
#include "Controller/GPlayerController.h"
#include "Item/GAOEActor.h"
#include "Item/GHomingProjectileActor.h"
#include "Item/GProjectileActor.h"
#include "Item/GSpinningProjectileActor.h"
#include "Item/GWeaponActor.h"
#include "Item/GWindProjectileActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnitConversion.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleSystemComponent.h"
#include "UI/GHUD.h"
#include "UI/GW_HPBar.h"

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

	// // //WidgetT* CreateWidget(OwnerType OwningObject, TSubclassOf<UUserWidget> UserWidgetClass = WidgetT::StaticClass(), FName WidgetName = NAME_None)
	// if(HasAuthority() == false)
	// {
	// 	BossHPBarWidgetRef = CreateWidget<UGW_HPBar>(GetWorld(), BossHPBarWidgetTemplate);
	// 	BossHPBarWidgetRef->OnCurrentHPChange(StatComponent->GetCurrentHP(), StatComponent->GetCurrentHP());
	// 	//바인드 해줘야 함
	// }
	
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

	if(HasAuthority() == false)
		return;
	
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

void AGBoss01::OnShootProjectile()
{
	Super::OnShootProjectile();

	if(HasAuthority() == false)
		return;
	
	UKismetSystemLibrary::PrintString(this, TEXT("OnShootProjectile is called"));

	// 발사 방향 추출
	AGAIController* AIController = Cast<AGAIController>(GetController());
	if(AIController == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("AIController is Invalid"));
		return;
	}
	//AGCharacter* TargetActor = Cast<AGCharacter>(AIController->TargetActor);
	AGCharacter* TargetActor = Cast<AGCharacter>(AIController->GetBlackboardComponent()->GetValueAsObject(AGAIController::TargetActorKey));
	if(TargetActor == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("TargetActor is Invalid"));
		return;
	}
	
	if(IsValid(AIController) == true && IsValid(TargetActor) == true)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(
											TEXT("Target Actor Name: %s"), *TargetActor->GetName()));
		
		//UKismetSystemLibrary::PrintString(this, TEXT("AIController & AIController is Vaild"));
		//FVector MuzzleLocation = WeaponInstance->GetArrowSpawnArrowComponent()->GetComponentLocation();
		FName WeaponSocket(TEXT("Weapon_Socket_R"));
		FVector MuzzleLocation;
		if (GetMesh()->DoesSocketExist(WeaponSocket) == true)
		{
			MuzzleLocation = GetMesh()->GetSocketLocation(WeaponSocket);
			UKismetSystemLibrary::PrintString(this, TEXT("MuzzleLocation is Vaild"));
		}
		FVector HitLocation = TargetActor->GetActorLocation();
		
		FVector LaunchDirection = HitLocation - MuzzleLocation;
		LaunchDirection.Normalize();
		FRotator LaunchRotation = LaunchDirection.Rotation();
		
		//DrawDebugSphere(GetWorld(), MuzzleLocation, 10.f, 16, FColor::Red, false, 10.f);
		//DrawDebugSphere(GetWorld(), HitLocation, 10.f, 16, FColor::Magenta, false, 10.f);
		//DrawDebugLine(GetWorld(), MuzzleLocation, HitLocation, FColor::Yellow, false, 10.f, 0, 1.f);
		
		// 발사
		if (IsValid(ProjectileClass) == true)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
			AGSpinningProjectileActor* SpawnedArrow = GetWorld()->SpawnActor<AGSpinningProjectileActor>(ProjectileClass, MuzzleLocation, LaunchRotation, SpawnParams);
			if (IsValid(SpawnedArrow) == true)
			{
				
				UKismetSystemLibrary::PrintString(this, TEXT("OnShoot is called"));
			}
		}
	}
}

void AGBoss01::OnShootWindProjectile()
{
	Super::OnShootWindProjectile();

	if(HasAuthority() == false)
		return;
	
	UKismetSystemLibrary::PrintString(this, TEXT("OnShootWindProjectile is called"));

	// 발사 방향 추출
	AGAIController* AIController = Cast<AGAIController>(GetController());
	if(AIController == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("AIController is Invalid"));
		return;
	}
	//AGCharacter* TargetActor = Cast<AGCharacter>(AIController->TargetActor);
	AGCharacter* TargetActor = Cast<AGCharacter>(AIController->GetBlackboardComponent()->GetValueAsObject(AGAIController::TargetActorKey));
	if(TargetActor == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("TargetActor is Invalid"));
		return;
	}
	
	if(IsValid(AIController) == true && IsValid(TargetActor) == true)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(
											TEXT("Target Actor Name: %s"), *TargetActor->GetName()));
		
		//UKismetSystemLibrary::PrintString(this, TEXT("AIController & AIController is Vaild"));
		//FVector MuzzleLocation = WeaponInstance->GetArrowSpawnArrowComponent()->GetComponentLocation();
		FName WeaponSocket(TEXT("Weapon_Socket_R"));
		FVector MuzzleLocation;
		if (GetMesh()->DoesSocketExist(WeaponSocket) == true)
		{
			MuzzleLocation = GetMesh()->GetSocketLocation(WeaponSocket);
			UKismetSystemLibrary::PrintString(this, TEXT("MuzzleLocation is Vaild"));
		}
		FVector HitLocation = TargetActor->GetActorLocation();
		
		FVector LaunchDirection = HitLocation - MuzzleLocation;
		LaunchDirection.Normalize();
		FRotator LaunchRotation = LaunchDirection.Rotation();
		
		//DrawDebugSphere(GetWorld(), MuzzleLocation, 10.f, 16, FColor::Red, false, 10.f);
		//DrawDebugSphere(GetWorld(), HitLocation, 10.f, 16, FColor::Magenta, false, 10.f);
		//DrawDebugLine(GetWorld(), MuzzleLocation, HitLocation, FColor::Yellow, false, 10.f, 0, 1.f);
		
		// 발사
		if (IsValid(WindProjectileClass) == true)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
			AGWindProjectileActor* SpawnedArrow = GetWorld()->SpawnActor<AGWindProjectileActor>(WindProjectileClass, MuzzleLocation, LaunchRotation, SpawnParams);
			if (IsValid(SpawnedArrow) == true)
			{
				
				UKismetSystemLibrary::PrintString(this, TEXT("OnShootWind is called"));
			}
		}
	}
}

void AGBoss01::OnShootMultipleProjectile()
{
	Super::OnShootMultipleProjectile();
	
	if(HasAuthority() == false)
		return;
	
	UKismetSystemLibrary::PrintString(this, TEXT("OnShootMultipleProjectile is called"));

	// 발사 방향 추출
	AGAIController* AIController = Cast<AGAIController>(GetController());
	if(AIController == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("AIController is Invalid"));
		return;
	}
	//AGCharacter* TargetActor = Cast<AGCharacter>(AIController->TargetActor);
	AGCharacter* TargetActor = Cast<AGCharacter>(AIController->GetBlackboardComponent()->GetValueAsObject(AGAIController::TargetActorKey));
	if(TargetActor == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("TargetActor is Invalid"));
		return;
	}
	
	if(IsValid(AIController) == true && IsValid(TargetActor) == true)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(
											TEXT("Target Actor Name: %s"), *TargetActor->GetName()));
		
		//UKismetSystemLibrary::PrintString(this, TEXT("AIController & AIController is Vaild"));
		//FVector MuzzleLocation = WeaponInstance->GetArrowSpawnArrowComponent()->GetComponentLocation();
		FName WeaponSocket(TEXT("Weapon_Socket_R"));
		FVector MuzzleLocation;
		if (GetMesh()->DoesSocketExist(WeaponSocket) == true)
		{
			MuzzleLocation = GetMesh()->GetSocketLocation(WeaponSocket);
			UKismetSystemLibrary::PrintString(this, TEXT("MuzzleLocation is Vaild"));
		}
		FVector HitLocation = TargetActor->GetActorLocation();

		// 3발의 프로젝타일을 각각 발사
		for (int32 i = 0; i < 3; i++)
		{
			FVector LaunchDirection = HitLocation - MuzzleLocation;
			LaunchDirection.Normalize();
			FRotator LaunchRotation = LaunchDirection.Rotation();
			
			if (i == 1) // 우측 발사
			{
				LaunchRotation.Yaw += 45.f;
			}
			else if (i == 2) // 좌측 발사
			{
				LaunchRotation.Yaw -= 45.f;
			}

			// 프로젝타일 생성
			if (IsValid(MultipleProjectileClass) == true)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
				AGHomingProjectileActor* SpawnedHomingArrow = GetWorld()->SpawnActor<AGHomingProjectileActor>(MultipleProjectileClass, MuzzleLocation, LaunchRotation, SpawnParams);
				if (IsValid(SpawnedHomingArrow) == true)
				{
					//SpawnedHomingArrow->InitializeHoming(TargetActor);
					SpawnedHomingArrow->EnableHoming(TargetActor, 2.0f);
					UKismetSystemLibrary::PrintString(this, TEXT("OnShootMultipleProjectile HOMING is called"));
				}
			}
		}
	}
	
}

void AGBoss01::OnShootShapeAOE()
{
	Super::OnShootShapeAOE();
}

void AGBoss01::OnShootLaser()
{
	Super::OnShootLaser();

	//
}

void AGBoss01::OnShootAOE()
{
	Super::OnShootAOE();

	if(HasAuthority() == false)
		return;
	
	UKismetSystemLibrary::PrintString(this, TEXT("OnShootAOE is called"));

	// 발사 방향 추출
	AGAIController* AIController = Cast<AGAIController>(GetController());
	if(AIController == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("AIController is Invalid"));
		return;
	}
	//AGCharacter* TargetActor = Cast<AGCharacter>(AIController->TargetActor);
	AGCharacter* TargetActor = Cast<AGCharacter>(AIController->GetBlackboardComponent()->GetValueAsObject(AGAIController::TargetActorKey));
	if(TargetActor == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("TargetActor is Invalid"));
		return;
	}
	
	if(IsValid(AIController) == true && IsValid(TargetActor) == true)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(
											TEXT("Target Actor Name: %s"), *TargetActor->GetName()));
		
		
		// 발사
		if (IsValid(AOEClass) == true)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
			AGAOEActor* SpawnedAOE = GetWorld()->SpawnActor<AGAOEActor>(AOEClass, TargetActor->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
			if (IsValid(SpawnedAOE) == true)
			{
				
				UKismetSystemLibrary::PrintString(this, TEXT("OnShoot is called"));
			}
		}
	}
}

void AGBoss01::BeginShoot()
{
	Super::BeginShoot();
	
	UKismetSystemLibrary::PrintString(this, TEXT("BeginShoot is called"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	bIsNowAttacking = true;
	
	PlayShootAnimMontage_NetMulticast();
}

void AGBoss01::PlayShootAnimMontage_NetMulticast_Implementation()
{
	UKismetSystemLibrary::PrintString(this, TEXT("PlayShootAnimMontage is called by NetMulticast"));

	// 애님 몽타주 재생
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	AnimInstance->PlayAnimMontage(ShootMontage);

	if (OnShootMontageEndedDelegate.IsBound() == false)
	{
		OnShootMontageEndedDelegate.BindUObject(this, &ThisClass::EndShoot);
		AnimInstance->Montage_SetEndDelegate(OnShootMontageEndedDelegate, ShootMontage);
	}
	AnimInstance->Montage_SetEndDelegate(OnShootMontageEndedDelegate_Task, ShootMontage);

}

void AGBoss01::EndShoot(UAnimMontage* InMontage, bool bInterruped)
{
	Super::EndShoot(InMontage, bInterruped);

	UKismetSystemLibrary::PrintString(this, TEXT("EndShoot is called"));
	
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bIsNowAttacking = false;

	if (OnShootMontageEndedDelegate.IsBound() == true)
	{
		OnShootMontageEndedDelegate.Unbind();
	}
}

void AGBoss01::BeginShootWind()
{
	Super::BeginShootWind();

	UKismetSystemLibrary::PrintString(this, TEXT("BeginShootWind is called"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	bIsNowAttacking = true;
	
	PlayShootWindAnimMontage_NetMulticast();
}

void AGBoss01::PlayShootWindAnimMontage_NetMulticast_Implementation()
{
	UKismetSystemLibrary::PrintString(this, TEXT("PlayShootWindAnimMontage is called by NetMulticast"));

	// 애님 몽타주 재생
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	AnimInstance->PlayAnimMontage(ShootWindMontage);

	if (OnShootWindMontageEndedDelegate.IsBound() == false)
	{
		OnShootWindMontageEndedDelegate.BindUObject(this, &ThisClass::EndShootWind);
		AnimInstance->Montage_SetEndDelegate(OnShootWindMontageEndedDelegate, ShootWindMontage);
	}
	AnimInstance->Montage_SetEndDelegate(OnShootWindMontageEndedDelegate_Task, ShootWindMontage);
}

void AGBoss01::EndShootWind(UAnimMontage* InMontage, bool bInterruped)
{
	Super::EndShootWind(InMontage, bInterruped);
	
	UKismetSystemLibrary::PrintString(this, TEXT("EndShootWind is called"));
	
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bIsNowAttacking = false;

	if (OnShootWindMontageEndedDelegate.IsBound() == true)
	{
		OnShootWindMontageEndedDelegate.Unbind();
	}
}

void AGBoss01::BeginShootMultiple()
{
	Super::BeginShootMultiple();

	UKismetSystemLibrary::PrintString(this, TEXT("BeginShootMultiple is called"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	bIsNowAttacking = true;
	
	PlayShootMultipleAnimMontage_NetMulticast();
}

void AGBoss01::PlayShootMultipleAnimMontage_NetMulticast_Implementation()
{
	UKismetSystemLibrary::PrintString(this, TEXT("PlayShootMultipleAnimMontage is called by NetMulticast"));

	// 애님 몽타주 재생
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	AnimInstance->PlayAnimMontage(ShootMultipleMontage);

	if (OnShootMultipleMontageEndedDelegate.IsBound() == false)
	{
		OnShootMultipleMontageEndedDelegate.BindUObject(this, &ThisClass::EndShootMultiple);
		AnimInstance->Montage_SetEndDelegate(OnShootMultipleMontageEndedDelegate, ShootMultipleMontage);
	}
	AnimInstance->Montage_SetEndDelegate(OnShootMultipleMontageEndedDelegate_Task, ShootMultipleMontage);
}

void AGBoss01::EndShootMultiple(UAnimMontage* InMontage, bool bInterruped)
{
	Super::EndShootMultiple(InMontage, bInterruped);

	UKismetSystemLibrary::PrintString(this, TEXT("EndShootMultiple is called"));
	
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bIsNowAttacking = false;

	if (OnShootMultipleMontageEndedDelegate.IsBound() == true)
	{
		OnShootMultipleMontageEndedDelegate.Unbind();
	}
}

void AGBoss01::BeginShootAOE()
{
	Super::BeginShootAOE();
	
	UKismetSystemLibrary::PrintString(this, TEXT("BeginShootAOE is called"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	bIsNowAttacking = true;
	
	PlayShootAOEAnimMontage_NetMulticast();
}

void AGBoss01::PlayShootAOEAnimMontage_NetMulticast_Implementation()
{
	UKismetSystemLibrary::PrintString(this, TEXT("PlayShootAOEAnimMontage is called by NetMulticast"));

	// 애님 몽타주 재생
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	AnimInstance->PlayAnimMontage(ShootAOEMontage);

	if (OnShootAOEMontageEndedDelegate.IsBound() == false)
	{
		OnShootAOEMontageEndedDelegate.BindUObject(this, &ThisClass::EndShootAOE);
		AnimInstance->Montage_SetEndDelegate(OnShootAOEMontageEndedDelegate, ShootAOEMontage);
	}
	AnimInstance->Montage_SetEndDelegate(OnShootAOEMontageEndedDelegate_Task, ShootAOEMontage);
}

void AGBoss01::EndShootAOE(UAnimMontage* InMontage, bool bInterruped)
{
	Super::EndShootAOE(InMontage, bInterruped);

	UKismetSystemLibrary::PrintString(this, TEXT("EndShootAOE is called"));
	
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bIsNowAttacking = false;

	if (OnShootAOEMontageEndedDelegate.IsBound() == true)
	{
		OnShootAOEMontageEndedDelegate.Unbind();
	}
}

void AGBoss01::BeginShootLaser()
{
	Super::BeginShootLaser();

	UKismetSystemLibrary::PrintString(this, TEXT("BeginShootLaser is called"));
	
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	bIsNowAttacking = true;
	
	PlayShootLaserAnimMontage_NetMulticast();
}

void AGBoss01::PlayShootLaserAnimMontage_NetMulticast_Implementation()
{
	UKismetSystemLibrary::PrintString(this, TEXT("PlayShootLaserAnimMontage is called by NetMulticast"));

	// 애님 몽타주 재생
	UGAnimInstance* AnimInstance = Cast<UGAnimInstance>(GetMesh()->GetAnimInstance());
	ensureMsgf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	AnimInstance->PlayAnimMontage(ShootLaserMontage);

	if (OnShootLaserMontageEndedDelegate.IsBound() == false)
	{
		OnShootLaserMontageEndedDelegate.BindUObject(this, &ThisClass::EndShootLaser);
		AnimInstance->Montage_SetEndDelegate(OnShootLaserMontageEndedDelegate, ShootLaserMontage);
	}
	AnimInstance->Montage_SetEndDelegate(OnShootLaserMontageEndedDelegate_Task, ShootLaserMontage);
}

void AGBoss01::EndShootLaser(UAnimMontage* InMontage, bool bInterruped)
{
	Super::EndShootLaser(InMontage, bInterruped);
	
	UKismetSystemLibrary::PrintString(this, TEXT("EndShootLaser is called"));
	
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bIsNowAttacking = false;

	if (OnShootLaserMontageEndedDelegate.IsBound() == true)
	{
		OnShootLaserMontageEndedDelegate.Unbind();
	}
}

void AGBoss01::OnJump()
{
	Super::OnJump();

	// Server, Client 모두
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	// OnLand하면, 되돌려야 함
	// 혹은 OnLand AN을 만들어야 함
	// 혹은 ANS로 활용

	if(HasAuthority() != true)
		return;

	// Server 에서만 처리
	
	UKismetSystemLibrary::PrintString(this, TEXT("OnJump is called"));

	AGAIController* AIController = Cast<AGAIController>(GetController());
	if(AIController == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("AIController is Invalid"));
		return;
	}
	//AGCharacter* TargetActor = Cast<AGCharacter>(AIController->TargetActor);
	AGCharacter* TargetActor = Cast<AGCharacter>(AIController->GetBlackboardComponent()->GetValueAsObject(AGAIController::TargetActorKey));
	if(TargetActor == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("TargetActor is Invalid"));
		return;
	}

	FVector StartLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	TargetLocation.Z += 100.f;

	// 예측
	float CalculateTime = 1.0f;
	FVector TargetVelocity = TargetActor->GetVelocity();
	FVector PredictedVelocity = TargetVelocity * FVector(1.0f, 1.0f, 0.0f) * CalculateTime;
	FVector PredictedLocation = TargetActor->GetActorLocation() + PredictedVelocity;

	// 타겟의 앞쪽에 도착하기위한 부분
	FVector DirectionToTarget = (PredictedLocation - StartLocation).GetSafeNormal();
	float OffsetDistance = 200.0f;
	PredictedLocation -= DirectionToTarget * OffsetDistance;

	// Arc 조절
	float arcValue = 0.5f;// ArcParam (0.0-1.0)
	FVector OutVelocity = FVector::ZeroVector;// 결과 Velocity

	bool bResult = UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutVelocity, StartLocation, PredictedLocation, GetWorld()->GetGravityZ(), arcValue);

	if (bResult)
	{
		FPredictProjectilePathParams predictParams(20.0f, StartLocation, OutVelocity, 5.0f);//20: tracing 보여질 프로젝타일 크기, 15: 시뮬레이션되는 Max 시간(초)
		predictParams.DrawDebugTime = 5.0f;
		predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;// DrawDebugTime 을 지정하면 EDrawDebugTrace::Type::ForDuration 필요
		predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
		FPredictProjectilePathResult result;
		UGameplayStatics::PredictProjectilePath(this, predictParams, result);

		LaunchCharacter(OutVelocity, true, true);
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Failed to calculate projectile velocity!"));
	}
	
}

void AGBoss01::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	
	// 서버 공통
	if(HasAuthority() == true)
	{
		
	}
	// Client 공통
	else
	{
		
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

	// AGAIController* AIController = Cast<AGAIController>(GetController());
	// AGPlayerCharacter* Target = Cast<AGPlayerCharacter>(AIController->TargetActor);
	// AGPlayerController* TargetController = Cast<AGPlayerController>(Target->GetController());
	// UGHUD* TargetHUD = TargetController->GetHUDWidget();
	// UVerticalBox* TargetHUDTopVerticalBox = TargetHUD->GetTopVerticalBox();
	// TargetHUDTopVerticalBox->AddChildToVerticalBox(BossHPBarWidgetRef);
	
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
