// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GWeaponActor.h"

// Sets default values
AGWeaponActor::AGWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	//SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AGWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(Mesh != nullptr, TEXT("Invalid Mesh"));
	ensureMsgf(UnarmedCharacterAnimLayer != nullptr, TEXT("Invalid UnarmedCharacterAnimLayer"));
	ensureMsgf(ArmedCharacterAnimLayer != nullptr, TEXT("ArmedCharacterAnimLayer"));
	ensureMsgf(EquipAnimMontage != nullptr, TEXT("EquipAnimMontage"));
	ensureMsgf(UnequipAnimMontage != nullptr, TEXT("UnequipAnimMontage"));
	
}

// Called every frame
void AGWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

