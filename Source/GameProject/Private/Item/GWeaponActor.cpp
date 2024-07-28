// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GWeaponActor.h"

#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGWeaponActor::AGWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	//Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponNumber = 0;

	DirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	DirectionArrow->SetupAttachment(Mesh);
}

void AGWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

// Called when the game starts or when spawned
void AGWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(IsValid(Mesh), TEXT("Invalid Mesh"));
	//ensureMsgf(IsValid(UnarmedCharacterAnimLayer), TEXT("Invalid UnarmedCharacterAnimLayer"));
	ensureMsgf(IsValid(ArmedCharacterAnimLayer), TEXT("Invalid ArmedCharacterAnimLayer"));
	//ensureMsgf(IsValid(EquipAnimMontage), TEXT("Invalid EquipAnimMontage"));
	//ensureMsgf(IsValid(UnequipAnimMontage), TEXT("Invalid UnequipAnimMontage"));
	//ensureMsgf(IsValid(BasicAttackAnimMontage), TEXT("Invalid BasicAttackAnimMontage"));

	ensureMsgf(WeaponNumber != 0, TEXT("Invalid WeaponNumber"));
}

// Called every frame
void AGWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}
