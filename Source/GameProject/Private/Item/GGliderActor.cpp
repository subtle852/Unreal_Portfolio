// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GGliderActor.h"

// Sets default values
AGGliderActor::AGGliderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	//SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AGGliderActor::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(IsValid(Mesh), TEXT("Invalid Mesh"));
	ensureMsgf(IsValid(UnarmedCharacterAnimLayer), TEXT("Invalid UnarmedCharacterAnimLayer"));
}

// Called every frame
void AGGliderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

