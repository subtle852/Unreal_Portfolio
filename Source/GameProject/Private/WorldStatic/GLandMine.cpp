// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/GLandMine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AGLandMine::AGLandMine()
	: bIsExploded(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyBoxComponent"));
	SetRootComponent(BodyBoxComponent);

	BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
	BodyStaticMeshComponent->SetupAttachment(GetRootComponent());

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(GetRootComponent());
	ParticleSystemComponent->SetAutoActivate(false);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AGLandMine::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(IsValid(ExplodedMaterial), TEXT("Invalid ExplodedMaterial"));
	
	if (false == OnActorBeginOverlap.IsAlreadyBound(this, &ThisClass::OnLandMineBeginOverlap))
	{
		OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnLandMineBeginOverlap);
	}
}

void AGLandMine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (true == OnActorBeginOverlap.IsAlreadyBound(this, &ThisClass::OnLandMineBeginOverlap))
	{
		OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnLandMineBeginOverlap);
	}
	
	Super::EndPlay(EndPlayReason);
	
}

// Called every frame
void AGLandMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGLandMine::OnLandMineBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// if (HasAuthority() == true)
	// {
	// 	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AGLandMine::OnLandMineBeginOverlap() has been called in Server PC.")));
	// }
	// else
	// {
	// 	if (GetOwner() == UGameplayStatics::GetPlayerController(this, 0))
	// 	{
	// 		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AGLandMine::OnLandMineBeginOverlap() has been called in OwningClient PC.")));
	// 	}
	// 	else
	// 	{
	// 		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AGLandMine::OnLandMineBeginOverlap() has been called in OtherClient PC.")));
	// 	}
	// }

	if (HasAuthority() == true && bIsExploded == false)
	{
		SpawnEffect_NetMulticast();
		bIsExploded = true;
	}
}

void AGLandMine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsExploded);
}

void AGLandMine::OnRep_IsExploded()
{
	if (bIsExploded == true)
	{
		BodyStaticMeshComponent->SetMaterial(0, ExplodedMaterial);
	}
}

void AGLandMine::SpawnEffect_NetMulticast_Implementation()
{
	if (HasAuthority() == false)
	{
		ParticleSystemComponent->Activate(true);
		
		BodyStaticMeshComponent->SetMaterial(0, ExplodedMaterial);
	}
}

bool AGLandMine::SpawnEffect_NetMulticast_Validate()
{
	return true;
}

