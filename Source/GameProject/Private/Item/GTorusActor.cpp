// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GTorusActor.h"

// Sets default values
AGTorusActor::AGTorusActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
	BodyStaticMeshComponent->SetupAttachment(Root);
	
	// 충돌 설정
	//BodyStaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AGTorusActor::OnOverlapBegin);
	
	GrowthRate = 8.f;
	MaxScale = 50.0f;
	CurrentScale = 1.0f;
}

void AGTorusActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called when the game starts or when spawned
void AGTorusActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGTorusActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority() == true)
	{
		if (CurrentScale < MaxScale)
		{
			CurrentScale += GrowthRate * DeltaTime;
			FVector NewScale(CurrentScale, CurrentScale, 1.0f);
            
			// Server에서 Scale 지정하고 Server, 모든 Client에 적용
			ApplyScale_NetMulticast(NewScale);
		}
		else
		{
			// if(CurrentScale >= MaxScale)
			// {
			// 	Destroy();
			// }
		}
	}
}

void AGTorusActor::ApplyScale_NetMulticast_Implementation(FVector InScale)
{
	BodyStaticMeshComponent->SetWorldScale3D(InScale);
}

void AGTorusActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                         FVector NormalImpulse, const FHitResult& Hit)
{
}

void AGTorusActor::OnHit_Server_Implementation(FVector InNewLocation)
{
}

void AGTorusActor::OnHit_NetMulticast_Implementation(FVector InNewLocation)
{
}

void AGTorusActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AGTorusActor::OnBeginOverlap_Server_Implementation()
{
}

void AGTorusActor::OnBeginOverlap_NetMulticast_Implementation()
{
}
