// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"
#include "PlayerInfoWidget.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NetworkProject/NetworkProjectCharacter.h"

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
	auto owningPawn = Cast<APawn>(GetOwner());

	if(owningPawn->GetController() != nullptr && owningPawn->GetController()->IsLocalController())
	{
		// 플레이어 컨트롤러에 소유되지 않기 때문에 오우너 설정을 해준다
		SpawnEmitter();
	}

	if(HasAuthority())
	{
		auto player = Cast<ANetworkProjectCharacter>(OtherActor);

		if(player != nullptr)
		{
			player->ServerDamageProcess(-10);
		}
	}
	
	// 부딪히면 불꽃 이펙트를 출력한 다음 제거한다
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireEffect, GetActorLocation());

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

