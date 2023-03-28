// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ABulletActor::ABulletActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
	SetRootComponent(sphereComp);
	sphereComp->SetSphereRadius(15);
	sphereComp->SetCollisionProfileName(FName("BulletPreset"));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(sphereComp);
	meshComp->SetRelativeScale3D(FVector(0.3f));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();

	sphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABulletActor::OnOverlap);
}

// Called every frame
void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + GetActorForwardVector() * moveSpeed * DeltaTime);

}

void ABulletActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// �ε����� �Ҳ� ����Ʈ�� ����� ���� �����Ѵ�
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireEffect, GetActorLocation());

	// �÷��̾� ��Ʈ�ѷ��� �������� �ʱ� ������ ����� ������ ���ش�
	SpawnEmitter();

	//Destroy();
}

void ABulletActor::SpawnEmitter_Implementation()
{
	SpawnMultiEmitter();
}

void ABulletActor::SpawnMultiEmitter_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireEffect, GetActorLocation());
}
