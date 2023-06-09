// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"
#include "PlayerInfoWidget.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerState.h"
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
	if(GetOwner() == nullptr)
	{
		return;
	}

	auto player = Cast<ANetworkProjectCharacter>(OtherActor);

	if(OtherActor != GetOwner())
	{
		if(HasAuthority())
		{
			
			if(player != nullptr)
			{
				if(player->GetHealth() <= attackPower)
				{
					ANetworkProjectCharacter* myOwner = Cast<ANetworkProjectCharacter>(GetOwner());

					if(myOwner != nullptr)
					{
						myOwner->GetPlayerState()->SetScore(myOwner->GetPlayerState()->GetScore() + 10.0f);
					}
				}

				player->ServerDamageProcess(attackPower * -1);

				Destroy();
			}
		}
	}
}

void ABulletActor::Destroyed()
{
	Super::Destroyed();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireEffect, GetActorLocation());
}

