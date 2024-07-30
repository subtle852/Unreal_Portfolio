// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGProjectileActor::AGProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	//Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(false);// 중력 미사용 중

	Mesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(Mesh);
	ProjectileMovementComponent->InitialSpeed = 3000.f;
	ProjectileMovementComponent->MaxSpeed = 4000.f;
	//ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	

	LaunchSpeed = 50.f;
}

void AGProjectileActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
}

// Called when the game starts or when spawned
void AGProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(IsValid(Mesh), TEXT("Invalid Mesh"));

	Mesh->OnComponentHit.AddDynamic(this, &AGProjectileActor::OnHit);
	
}

// Called every frame
void AGProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnHit() has been called.")));

	if (OtherActor)
	{
		FString OtherActorName = OtherActor->GetName();
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor: %s"), *OtherActorName));
	}
	
	if (IsValid(ProjectileMovementComponent) == true)
	{
		ProjectileMovementComponent->StopMovementImmediately();// 화살 이동 멈춤

		Mesh->SetSimulatePhysics(false);// 물리 시뮬레이션 중지
		//Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);// 충돌 중지
		Mesh->AttachToComponent(HitComponent, FAttachmentTransformRules::KeepWorldTransform);// 충돌한 표면에 부착
	}
	
}

