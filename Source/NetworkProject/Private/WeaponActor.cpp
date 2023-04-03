// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "../NetworkProjectCharacter.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);
	boxComp->SetCollisionProfileName(FName("WeaponPreset"));
	boxComp->SetSimulatePhysics(true);

	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(boxComp);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnOverlap);
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	{
		ANetworkProjectCharacter* player = Cast<ANetworkProjectCharacter>(OtherActor);

		// 만일 총을 가진 플레이어가 아니라면
		if(player != nullptr && player->owningWeapon == nullptr)
		{
			player->ammo = ammo;
			ServerGrabWeapon(player);
		}
	}
}

void AWeaponActor::ServerReleaseWeapon_Implementation(ANetworkProjectCharacter* player)
{
	MulticastReleaseWeapon(player);
	SetOwner(nullptr);
	player->ammo = 0;
}

void AWeaponActor::MulticastReleaseWeapon_Implementation(ANetworkProjectCharacter* player)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	boxComp->SetSimulatePhysics(true);

	FTimerHandle releaseHandle;
	GetWorld()->GetTimerManager().SetTimer(releaseHandle, FTimerDelegate::CreateLambda([&]()
		{
			boxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
		}), 3.0f, false);
	player->owningWeapon = nullptr;
}

void AWeaponActor::ServerGrabWeapon_Implementation(ANetworkProjectCharacter* player)
{
	SetOwner(player);
	MulticastGrabWeapon(player);
}

void AWeaponActor::MulticastGrabWeapon_Implementation(ANetworkProjectCharacter* player)
{
	player->owningWeapon = this;
	boxComp->SetSimulatePhysics(false);
	AttachToComponent(player->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("WeaponSocket"));
	boxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
}

void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponActor, ammo);
	DOREPLIFETIME(AWeaponActor, reloadTime);
	DOREPLIFETIME(AWeaponActor, damagePower);
}
