// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GProjectileActor.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AGProjectileActor::AGProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(Root);
	BoxComponent->SetSimulatePhysics(false);// false로 해야 Homing이 가능한데 false로 하면, OnHit도 호출이 안되버림
	BoxComponent->SetNotifyRigidBodyCollision(true);
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetEnableGravity(false); // 중력 미사용 중
	BoxComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	
	//Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);//
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);// 중력 미사용 중
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(Root);//
	ProjectileMovementComponent->InitialSpeed = 4000.f;
	ProjectileMovementComponent->MaxSpeed = 5000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	
	ProjectileMovementComponent->bIsHomingProjectile = false;
	ProjectileMovementComponent->HomingAccelerationMagnitude = 50000.0f;
	
	ProjectileMovementComponent->SetIsReplicated(true);

	LaunchSpeed = 50.f;

	MaxLifetime = 15.0f;
	MaxDistance = 2000.0f;

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(Root);//
	ParticleSystemComponent->SetAutoActivate(false);

	HomingTarget = nullptr;

	// if(BoxComponent->OnComponentHit.IsBound() == false)
	// {
	// 	BoxComponent->OnComponentHit.AddDynamic(this, &AGProjectileActor::OnHit);
	// }
	//
	// if(BoxComponent->OnComponentBeginOverlap.IsBound() == false)
	// {
	// 	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGProjectileActor::OnBeginOverlap);
	// }
	
}

void AGProjectileActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGProjectileActor, HomingTarget);
	DOREPLIFETIME(AGProjectileActor, OwnerActor);
	//DOREPLIFETIME(AGProjectileActor, ProjectileMovementComponent);
	
}

// Called when the game starts or when spawned
void AGProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(IsValid(Mesh), TEXT("Invalid Mesh"));

	if (HasAuthority())
	{
		if (BoxComponent->OnComponentHit.IsBound() == false)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnHit() has been Bound")));
			BoxComponent->OnComponentHit.AddDynamic(this, &AGProjectileActor::OnHit);
		}

		if (BoxComponent->OnComponentBeginOverlap.IsBound() == false)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnComponentBeginOverlap() has been Bound")));
			BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGProjectileActor::OnBeginOverlap);
		}
	}

	OwnerActor = GetOwner();
	Lifetime = 0.0f;
	
}

// Called every frame
void AGProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority() == false)// 클라에서 처리
	{
		//HomingTarget 유도
		if (IsValid(HomingTarget))
		{
			// FVector HomingDirection = (HomingTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			// float Speed = ProjectileMovementComponent->Velocity.Size();
			// FVector NewVelocity = HomingDirection * Speed;
			//
			// UpdateVelocity_Server(NewVelocity);
		
		
			// FVector HomingDirection = (HomingTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			// FRotator TargetRotation = HomingDirection.Rotation();
			// UpdateRotation_Server(TargetRotation);
		
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Tick_HomingTarget has been called")));
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("HomingTarget Location is %s"), *HomingTarget->GetActorLocation().ToString()));
		}
		
	}
	
	if(HasAuthority() == true)
	{
		// 플레이어로부터의 거리 체크
		if (IsValid(OwnerActor))
		{
			float DistanceFromInstigator = FVector::Dist(GetActorLocation(), OwnerActor->GetActorLocation());
			if (DistanceFromInstigator > MaxDistance)
			{
				Destroy();
			}
		}

		// Lifetime 체크
		Lifetime += DeltaTime;
		if (Lifetime > MaxLifetime)
		{
			Destroy();
		}
	}
	
}

void AGProjectileActor::InitializeHoming(AActor* Target)
{
	if (IsValid(Target))
	{
		//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("InitializeHoming() has been called")));
		
		// HomingTarget = Target;
		// ProjectileMovementComponent->bIsHomingProjectile = true;
		// ProjectileMovementComponent->HomingTargetComponent = Target->GetRootComponent();

		//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("GetRootComponent Location is %s"), *Target->GetRootComponent()->GetName()));
		//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Target is %s"), *Target->GetName()));

		InitializeHoming_Server(Target);
	}
}

void AGProjectileActor::InitializeHoming_Server_Implementation(AActor* Target)
{
	HomingTarget = Target;
	ProjectileMovementComponent->bIsHomingProjectile = true;
	ProjectileMovementComponent->HomingTargetComponent = Target->GetRootComponent();
	
	InitializeHoming_NetMulticast(Target);
}

void AGProjectileActor::InitializeHoming_NetMulticast_Implementation(AActor* Target)
{
	if(HasAuthority() == true)
		return;

	HomingTarget = Target;
	ProjectileMovementComponent->bIsHomingProjectile = true;
	ProjectileMovementComponent->HomingTargetComponent = Target->GetRootComponent();
}

void AGProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	// Block 반응을 유발하는 바닥이나 벽과 같은 물체

	if (HasAuthority() == true)//  서버에서 처리
	{
		if(OtherActor != GetOwner())
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnHit() has been called in Server")));
		
			//FString OtherActorName = OtherActor->GetName();
			//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor: %s"), *OtherActorName));
			
			OnHit_Server(HitComponent);
		}
	}
	
}

void AGProjectileActor::OnHit_Server_Implementation(UPrimitiveComponent* InHitComponent)
{
	ProjectileMovementComponent->StopMovementImmediately();
			
	//Mesh->SetHiddenInGame(true);

	BoxComponent->AttachToComponent(InHitComponent, FAttachmentTransformRules::KeepWorldTransform);// 충돌한 표면에 부착
	BoxComponent->SetSimulatePhysics(false);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//BoxComponent->SetHiddenInGame(true);
	
	ParticleSystemComponent->Activate(true);
	if(ParticleSystemComponent->OnSystemFinished.IsBound() == false)
	{
		ParticleSystemComponent->OnSystemFinished.AddDynamic(this, &AGProjectileActor::OnEffectFinish);
	}

	SetActorEnableCollision(false);

	OnHit_NetMulticast(InHitComponent);
}

void AGProjectileActor::OnHit_NetMulticast_Implementation(UPrimitiveComponent* InHitComponent)
{
	if(HasAuthority() == true)
		return;
	
	ProjectileMovementComponent->StopMovementImmediately();
			
	//Mesh->SetHiddenInGame(true);

	BoxComponent->AttachToComponent(InHitComponent, FAttachmentTransformRules::KeepWorldTransform);// 충돌한 표면에 부착
	BoxComponent->SetSimulatePhysics(false);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//BoxComponent->SetHiddenInGame(true);
	
	ParticleSystemComponent->Activate(true);
	//ParticleSystemComponent->OnSystemFinished.AddDynamic(this, &AGProjectileActor::OnEffectFinish);

	SetActorEnableCollision(false);
}

void AGProjectileActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Overlap 반응을 유발하는 GCharacter 클래스와의 충돌
	
	if (HasAuthority() == true)// 서버에서 처리
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnBeginOverlap() has been called in Server")));
		
		if(OtherActor != GetOwner())
		{
			OnBeginOverlap_Server();

			// Damage_Server 로직 추가 예정
		}
	}
}

void AGProjectileActor::OnBeginOverlap_Server_Implementation()
{
	ProjectileMovementComponent->StopMovementImmediately();
	ProjectileMovementComponent->bIsHomingProjectile = false;
			
	//Mesh->AttachToComponent(HitComponent, FAttachmentTransformRules::KeepWorldTransform);// 충돌한 표면에 부착
	Mesh->SetHiddenInGame(true);

	BoxComponent->SetSimulatePhysics(false);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComponent->SetHiddenInGame(true);
			
	ParticleSystemComponent->Activate(true);
	if(ParticleSystemComponent->OnSystemFinished.IsBound() == false)
	{
		ParticleSystemComponent->OnSystemFinished.AddDynamic(this, &AGProjectileActor::OnEffectFinish);
	}

	SetActorEnableCollision(false);

	OnBeginOverlap_NetMulticast();
}

void AGProjectileActor::OnBeginOverlap_NetMulticast_Implementation()
{
	if (HasAuthority() == true)
		return;

	ProjectileMovementComponent->StopMovementImmediately();
	ProjectileMovementComponent->bIsHomingProjectile = false;

	//Mesh->AttachToComponent(HitComponent, FAttachmentTransformRules::KeepWorldTransform);// 충돌한 표면에 부착
	Mesh->SetHiddenInGame(true);

	BoxComponent->SetSimulatePhysics(false);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComponent->SetHiddenInGame(true);

	ParticleSystemComponent->Activate(true);
	//ParticleSystemComponent->OnSystemFinished.AddDynamic(this, &AGProjectileActor::OnEffectFinish);// 서버에서 처리

	SetActorEnableCollision(false);
}

void AGProjectileActor::OnEffectFinish(class UParticleSystemComponent* ParticleSystem)
{
	OnEffectFinish_Server_Implementation();
}

void AGProjectileActor::OnEffectFinish_Server_Implementation()
{
	Destroy();
}

