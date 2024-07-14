// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/GTorch.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
AGTorch::AGTorch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);

	BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
	BodyStaticMeshComponent->SetupAttachment(GetRootComponent());
	BodyStaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -30.f));

	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
	PointLightComponent->SetupAttachment(GetRootComponent());
	PointLightComponent->SetRelativeLocation(FVector(0.f, 0.f, 500.f));

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(GetRootComponent());
	ParticleSystemComponent->SetRelativeLocation(FVector(0.f, 0.f, 500.f));

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));

}

// Called when the game starts or when spawned
void AGTorch::BeginPlay()
{
	Super::BeginPlay();

	Super::PostInitializeComponents();

	RotationSpeed = 300.f;
	RotatingMovementComponent->RotationRate = FRotator(0.f, RotationSpeed, 0.f);
	
}

// Called every frame
void AGTorch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
