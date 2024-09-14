// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GLaserActor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "NiagaraComponent.h"

AGLaserActor::AGLaserActor()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	BoxComponent1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent1"));
	BoxComponent1->SetupAttachment(Root);
	
	BoxComponent2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent2"));
	BoxComponent2->SetupAttachment(Root);

	// BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
	// BodyStaticMeshComponent->SetupAttachment(Root);
	
	NiagaraSystemComponent1 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystemComponent1"));
	NiagaraSystemComponent1->SetupAttachment(Root);

	NiagaraSystemComponent2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystemComponent2"));
	NiagaraSystemComponent2->SetupAttachment(Root);
	
	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
	RotatingMovementComponent->SetUpdatedComponent(Root);
	RotatingMovementComponent->SetIsReplicated(true);

	RotationSpeed = 50.f;
	
}

void AGLaserActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
}

void AGLaserActor::BeginPlay()
{
	Super::BeginPlay();

	Super::PostInitializeComponents();

	if(HasAuthority() == true)
	{
		RotatingMovementComponent->RotationRate = FRotator(0.f, RotationSpeed, 0.f);
	}
}

void AGLaserActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGLaserActor::ApplyRotation_Implementation(FRotator InRotator)
{
}

void AGLaserActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
}

void AGLaserActor::OnHit_Server_Implementation(FVector InNewLocation)
{
}

void AGLaserActor::OnHit_NetMulticast_Implementation(FVector InNewLocation)
{
}

void AGLaserActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AGLaserActor::OnBeginOverlap_Server_Implementation()
{
}

void AGLaserActor::OnBeginOverlap_NetMulticast_Implementation()
{
}
