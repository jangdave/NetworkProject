// Fill out your copyright notice in the Description page of Project Settings.


#include "OwnershipActor.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NetworkProject/NetworkProjectCharacter.h"
#include "EngineUtils.h"

// Sets default values
AOwnershipActor::AOwnershipActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);
	boxComp->SetBoxExtent(FVector(50.0f));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(boxComp);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 서버 복제 on/off 함수
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AOwnershipActor::BeginPlay()
{
	Super::BeginPlay();

	// 만일 서버 액터라면
	//if(GetLocalRole() == ROLE_Authority) 아래랑 같은거
	if(HasAuthority())
	{
		// 랜덤한 방향을 설정한다
		moveDirection = FMath::VRand();
		moveDirection.Z = FMath::Abs(moveDirection.Z);

		// 5초에 한번씩 textNumber의 변수를 변경한다
		FTimerHandle repeatTimer;
		GetWorld()->GetTimerManager().SetTimer(repeatTimer, FTimerDelegate::CreateLambda([&]() {testNumber = FMath::Rand(); }), 5.0f, true);
	}

	// 매시의 재질을 다이나믹 메테리얼 인스턴스로 변경한다
	UMaterialInterface* base_mat = meshComp->GetMaterial(0);

	if(base_mat != nullptr)
	{
		mat = UMaterialInstanceDynamic::Create(base_mat, this);
		meshComp->SetMaterial(0, mat);
	}

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AOwnershipActor::OnOverlap);
}

// Called every frame
void AOwnershipActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 상태 정보를 출력한다
	DrawDebugString(GetWorld(), GetActorLocation(), PrintInfo(), nullptr, FColor::White, 0.0f, true, 1.0f);

	//SetActorLocation(GetActorLocation() + moveDirection * 100 * DeltaTime);

	if(HasAuthority())
	{
		CheckOwner();
	}

	DrawDebugSphere(GetWorld(), GetActorLocation(), 500.0f, 30, FColor::Cyan, false, 0.0f, 0, 1);
}

void AOwnershipActor::ScreenLog()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::FromInt(testNumber), true, FVector2D(1.5f));
}

FString AOwnershipActor::PrintInfo()
{
#pragma region RoleInfo
	FString myLocalRole = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	FString myRemoteRole = UEnum::GetValueAsString<ENetRole>(GetRemoteRole());
	FString myConnection = GetNetConnection() != nullptr ? TEXT("Valid") : TEXT("inValid");
	FString myOwner = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
	FString infoText = FString::Printf(TEXT("Local Role : %s\nRemote Role : %s\nNet Connection : %s\nOwener : %s"), *myLocalRole, *myRemoteRole, *myConnection, *myOwner);
#pragma endregion

#pragma region RoleInfo
	//FString infoText = FString::Printf(TEXT("%.2f, %.2f, %.2f"), moveDirection.X, moveDirection.Y, moveDirection.Z);

#pragma endregion

	return infoText;
}

// 지정된 범위 안에 플레이어가 있다면 가장 가까운 플레이어 캐릭터를 Owner로 설정한다
void AOwnershipActor::CheckOwner()
{
	float maxDistance = 500.0f;
	
	for(TActorIterator<ANetworkProjectCharacter> it(GetWorld()); it; ++it)
	{
		float distance = this->GetDistanceTo(*it);

		if(distance < maxDistance)
		{
			maxDistance = distance;

			player = *it;
		}
	}

	if(player != nullptr && GetOwner() != player)
	{
		SetOwner(player);
	}
}

// 서버에 복제 등록하기 위한 함수
void AOwnershipActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOwnershipActor, moveDirection);
	DOREPLIFETIME(AOwnershipActor, testNumber);
	DOREPLIFETIME(AOwnershipActor, matColor);
}

void AOwnershipActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	{
		//auto character = Cast<ANetworkProjectCharacter>(OtherActor);
		
		//if(character != nullptr)
		if(OtherActor->IsA<ANetworkProjectCharacter>())
		{
			// 메테리얼 색상을 랜덤하게 변경한다
			matColor = FMath::VRand();
			matColor = matColor.GetAbs();
			ChangeColor();
		}
	}
}

void AOwnershipActor::ChangeColor()
{
	// "mycolor" 파라미터 값을 matcolor 값으로 변경
	mat->SetVectorParameterValue(FName("MyColor"), (FLinearColor)matColor);
}

// 1.플레이어 캐릭터가 충돌하면 랜덤한 색상으로 변경한다
// 2.랜덤 색상은 모든 클라이언트에서 동일하게 보여야 한다