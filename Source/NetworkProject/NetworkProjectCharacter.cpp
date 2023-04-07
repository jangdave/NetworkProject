// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkProjectCharacter.h"
#include "BattleGameMode.h"
#include "BattlePlayerController.h"
#include "BattlePlayerState.h"
#include "BattleSpectatorPawn.h"
#include "BulletActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "PlayerAnimInstance.h"
#include "PlayerInfoWidget.h"
#include "ServerGameInstance.h"
#include "WeaponActor.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/HUD.h"
#include "Net/UnrealNetwork.h"


//////////////////////////////////////////////////////////////////////////
// ANetworkProjectCharacter

ANetworkProjectCharacter::ANetworkProjectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// 위젯 컴포넌트 생성
	playerInfoUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("playerInfoUI"));
	playerInfoUI->SetupAttachment(GetMesh());
}

void ANetworkProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if(HasAuthority())
	{
		SetHealth(maxHP);
	}

	infoWidget = Cast<UPlayerInfoWidget>(playerInfoUI->GetWidget());

	anim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	gameInstance = Cast<UServerGameInstance>(GetGameInstance());

	if(GetController() != nullptr && GetController()->IsLocalController())
	{
		SetServerName(gameInstance->sessionID.ToString());
	}

	//FTimerHandle nameHandle;
	/*GetWorldTimerManager().SetTimer(nameHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			infoWidget->text_Name->SetText(FText::FromString(myName));
		}), 0.5f, false);*/
}

void ANetworkProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	infoWidget->text_Name->SetText(FText::FromString(myName));

	if(bIsDead)
	{
		return;
	}

	// 상태 정보를 출력한다
	DrawDebugString(GetWorld(), GetActorLocation(), PrintInfo(), nullptr, FColor::White, 0.0f, true, 1.0f);

	if(HasAuthority())
	{
		number++;

		repNumber++;
	}
	
	infoWidget->SetHealth(curHP);

	if(curHP <= 0)
	{
		DieProcess();
	}
}

FString ANetworkProjectCharacter::PrintInfo()
{
	// 로그끄기 대용 변수
	FString infoText;
#pragma region RoleInfo
	//FString myLocalRole = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	//FString myRemoteRole = UEnum::GetValueAsString<ENetRole>(GetRemoteRole());
	//FString myConnection = GetNetConnection() != nullptr ? TEXT("Valid") : TEXT("inValid");
	//FString myOwner = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
	//FString name = this->GetName();
	//FString infoText = FString::Printf(TEXT("Local Role : %s\nRemote Role : %s\nNet Connection : %s\nOwener : %s\nName : %s"), *myLocalRole, *myRemoteRole, *myConnection, *myOwner, *name);
#pragma endregion

#pragma region RepOrNot
	//FString infoText = FString::Printf(TEXT("Number : %d\nReplicated Number : %d"), number, repNumber);
#pragma endregion

#pragma region PlayerInfo
	//APlayerController* pc = Cast<APlayerController>(GetController());

	//FString pcString = pc != nullptr ? FString(GetController()->GetName()) : FString("Has No Controller");
	//FString gmString = GetWorld()->GetAuthGameMode() != nullptr ? FString("Has GameModeBase") : FString("Has No GameModeBase");
	//FString gsString = GetWorld()->GetGameState() != nullptr ? FString("Has GameState") : FString("Has No GameState");
	//FString psString = GetPlayerState() != nullptr ? FString("Has PlayerState") : FString("Has No PlayerState");
	//FString hudString;

	//if(pc != nullptr)
	//{
	//	hudString = pc->GetHUD() != nullptr ? pc->GetHUD()->GetName() : FString("Has No HUD");
	//}

	//infoText = FString::Printf(TEXT("%s\n%s\n%s\n%s\n%s"), *pcString, *gmString, *gsString, *psString, *hudString);
#pragma endregion

	FString psName;
	if(GetPlayerState() != nullptr)
	{
		psName = GetPlayerState()->GetPlayerName();
	}

	FString gsName;
	if(GetWorld()->GetGameState() != nullptr)
	{
		for(TObjectPtr<APlayerState> ps : GetWorld()->GetGameState()->PlayerArray)
		{
			gsName.Append(FString::Printf(TEXT("%s\n"), *ps->GetPlayerName()));
		}
	}

	infoText = FString::Printf(TEXT("Player State : %s\nGame State : \n%s"), *psName, *gsName);

	return infoText;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetworkProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetworkProjectCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetworkProjectCharacter::Look);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ANetworkProjectCharacter::Fire);
		EnhancedInputComponent->BindAction(ReleaseAction, ETriggerEvent::Started, this, &ANetworkProjectCharacter::ReleaseWeapon);
	}
}

void ANetworkProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetworkProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ANetworkProjectCharacter::SetHealth(int32 value)
{
	curHP = FMath::Min(maxHP, value);
}

void ANetworkProjectCharacter::AddHealth(int32 value)
{
	curHP = FMath::Clamp(curHP + value, 0, maxHP);
}

void ANetworkProjectCharacter::EndSession()
{
	if(HasAuthority())
	{
		for(TActorIterator<ANetworkProjectCharacter> pl(GetWorld()); pl; ++pl)
		{
			ANetworkProjectCharacter* p = *pl;

			if(p != this)
			{
				p->ServerDestroyAllSessions();
			}
		}
		FTimerHandle testHandle;
		GetWorldTimerManager().SetTimer(testHandle, this, &ANetworkProjectCharacter::DestroySession, 1.0f, false);
	}
	else
	{
		DestroySession();
	}
}

void ANetworkProjectCharacter::DestroySession()
{
	gameInstance->sessionInterface->DestroySession(gameInstance->sessionID);

	// 레벨을 다시 처음 위치로 이동
	ABattlePlayerController* pc = Cast<ABattlePlayerController>(GetController());
	pc->ClientTravel(FString("/Game/Maps/LoginMap'"), ETravelType::TRAVEL_Relative);
}

void ANetworkProjectCharacter::ServerDestroyAllSessions_Implementation()
{
	MulticastDestroyAllSessions();
}

void ANetworkProjectCharacter::MulticastDestroyAllSessions_Implementation()
{
	if(GetController() != nullptr && GetController()->IsLocalController())
	{
		DestroySession();
	}
}

// 캐릭터 사망시 처리 함수
void ANetworkProjectCharacter::DieProcess()
{
	bIsDead = true;

	// 조작을 하는 클라이언트에서만 실행한다
	if (GetController() != nullptr)
	{
		GetCharacterMovement()->DisableMovement();

		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bUseControllerRotationYaw = false;
		FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 1);
		ReleaseWeapon();
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	}

	/*if(HasAuthority())
	{
		FTimerHandle respawnHandle;
		GetWorldTimerManager().SetTimer(respawnHandle, FTimerDelegate::CreateLambda([&]()
		{
			ChangeSpectatorMode();

			//Cast<ABattlePlayerController>(GetController())->Respawn(this);
		}), 3.0f, false);
	}*/
}

// 관전자 모드로 변경하는 함수
void ANetworkProjectCharacter::ChangeSpectatorMode()
{
	ABattleGameMode* gm = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());
	
	if(gm != nullptr)
	{
		// 게임 모드에 설정한 관전자 폰 클래스를 불러온다
		TSubclassOf<ASpectatorPawn> spectatorPawn = gm->SpectatorClass;

		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ABattleSpectatorPawn* spectator = GetWorld()->SpawnActor<ABattleSpectatorPawn>(spectatorPawn, GetActorLocation(), GetActorRotation(), param);

		if(spectator != nullptr)
		{
			spectator->originalPlayer = this;
			GetController()->Possess(spectator);
		}
	}
}

void ANetworkProjectCharacter::SetServerName_Implementation(const FString& name)
{
	myName = name;

	// 플레이어 스테이트에 세션id 값을 넣어준다
	ABattlePlayerState* ps = Cast<ABattlePlayerState>(GetPlayerState());

	if (ps != nullptr)
	{
		// 해당 플레이어에 대한 이름 변경
		ps->SetPlayerName(name);
	}
}

void ANetworkProjectCharacter::ServerDamageProcess_Implementation(int32 value)
{
	AddHealth(value);

	MulticastDamageProcess();

}

void ANetworkProjectCharacter::MulticastDamageProcess_Implementation()
{
	if (curHP > 0)
	{
		if (HitMontage != nullptr)
		{
			PlayAnimMontage(HitMontage);
		}
	}
	else
	{
		// 죽음 변수 변경

		if (anim != nullptr)
		{
			anim->bIsDead = true;
		}
	}
}

void ANetworkProjectCharacter::SpawnBullet()
{
	FVector loc = GetActorLocation() + GetActorForwardVector() * 100.0f;
	bullet = GetWorld()->SpawnActor<ABulletActor>(bulletFactory, owningWeapon->meshComp->GetSocketLocation(FName("AmmoSocket")), owningWeapon->meshComp->GetSocketRotation(FName("AmmoSocket")));
}

// 해킹 방지, 다 만들고 추가하는게 좋다, 서버 함수에만 가능
bool ANetworkProjectCharacter::ServerFire_Validate()
{
	//return damage <= 1000;
	return true;
}

// 총 발사
void ANetworkProjectCharacter::Fire()
{
	if(owningWeapon != nullptr && !bFireDelay)
	{
		UE_LOG(LogTemp, Warning, TEXT("Query Fire"));
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("Query Fire"), true, FVector2D(1.2f));

		// 지연시간이 있어서 평소 알던 함수 실행 순서랑 다르다
		ServerFire();
	}
}

// 서버에 요청하는 함수
void ANetworkProjectCharacter::ServerFire_Implementation()
{
	if(ammo > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server Fire"));
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("Server Fire"), true, FVector2D(1.2f));

		// 서버에서 구현
		SpawnBullet();

		bullet->SetOwner(this);
		bullet->attackPower = owningWeapon->damagePower;
		ammo--;
		owningWeapon->ammo--;

		bFireDelay = true;

		FTimerHandle fireDelayhandle;

		GetWorld()->GetTimerManager().SetTimer(fireDelayhandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			bFireDelay = false;
		}), owningWeapon->reloadTime, false);

		// 오너 해제
		//bullet->SetOwner(nullptr);

		MulticastFire(true);
		//ClientFire();
	}
	else
	{
		MulticastFire(false);
	}
}

// 서버로 부터 전달되는 함수
void ANetworkProjectCharacter::MulticastFire_Implementation(bool bHasAmmo)
{
	UE_LOG(LogTemp, Warning, TEXT("Multicast Fire"));
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("Multicast Fire"), true, FVector2D(1.2f));
	if(bHasAmmo)
	{
		if(FireMontage != nullptr)
		{
			PlayAnimMontage(FireMontage);
		}
	}
	else
	{
		if(NoAmmoMontage != nullptr)
		{
			PlayAnimMontage(NoAmmoMontage);
		}
	}
}

void ANetworkProjectCharacter::ClientFire_Implementation(int32 damage)
{
	UE_LOG(LogTemp, Warning, TEXT("Client Fire"));
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("Client Fire"), true, FVector2D(1.2f));

	//SpawnBullet();
}

// 총 내려놓기
void ANetworkProjectCharacter::ReleaseWeapon()
{
	if(GetController() != nullptr && GetController()->IsLocalController() && owningWeapon != nullptr)
	{
		owningWeapon->ServerReleaseWeapon(this);
	}
}

void ANetworkProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ANetworkProjectCharacter, repNumber);
	//DOREPLIFETIME_CONDITION(ANetworkProjectCharacter, repNumber, COND_OwnerOnly);

	DOREPLIFETIME(ANetworkProjectCharacter, curHP);
	DOREPLIFETIME(ANetworkProjectCharacter, ammo);
	DOREPLIFETIME(ANetworkProjectCharacter, myName);
	DOREPLIFETIME(ANetworkProjectCharacter, bFireDelay);
}
