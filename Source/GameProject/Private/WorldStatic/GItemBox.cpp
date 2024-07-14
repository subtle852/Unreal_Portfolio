#include "WorldStatic/GItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

AGItemBox::AGItemBox()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    SetRootComponent(BoxComponent);
    BoxComponent->SetBoxExtent(FVector(40.0f, 40.0f, 40.0f));
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGItemBox::OnOverlapBegin);
    //BoxComponent->SetCollisionProfileName(FName("Trigger"));
    BoxComponent->SetCollisionProfileName(FName("GItemBox"));

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    StaticMeshComponent->SetupAttachment(GetRootComponent());
    StaticMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -30.0f));
    StaticMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
    StaticMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(GetRootComponent());
    ParticleSystemComponent->SetAutoActivate(false);
}

void AGItemBox::BeginPlay()
{
    Super::BeginPlay();

    ensureMsgf(IsValid(StaticMeshComponent->GetStaticMesh()), TEXT("Invalid Mesh"));
    ensureMsgf(IsValid(ParticleSystemComponent->Template), TEXT("Invalid ParticleTemplate"));
}

void AGItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
    ParticleSystemComponent->Activate(true);
    StaticMeshComponent->SetHiddenInGame(true);
    SetActorEnableCollision(false);
    ParticleSystemComponent->OnSystemFinished.AddDynamic(this, &AGItemBox::OnEffectFinish);
}

void AGItemBox::OnEffectFinish(UParticleSystemComponent* ParticleSystem)
{
    Destroy();
}
