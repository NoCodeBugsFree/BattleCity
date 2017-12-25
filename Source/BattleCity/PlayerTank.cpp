// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerTank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PaperFlipbookComponent.h"
#include "ConstructorHelpers.h"
#include "PaperFlipbook.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectile.h"
#include "AIController.h"
#include "PickupBase.h"
#include "BattleCityGameModeBase.h"
#include "BattleCityGameState.h"
#include "BattleCityPlayerState.h"
#include "ExplodeFX.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

APlayerTank::APlayerTank()
{	
	/** invulnerable flipbook  */
	InvulnerableFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Invulnerable Flipbook"));
	InvulnerableFlipbook->SetupAttachment(RootComponent);
	InvulnerableFlipbook->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
	InvulnerableFlipbook->SetVisibility(false);
	InvulnerableFlipbook->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InvulnerableFlipbook->SetIsReplicated(true);

	/** camera boom  */
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->CameraLagSpeed = 2.f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetWorldRotation(FRotator(-90.f, 0.0f, 0.0f));
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = false;

	/** camera  */
	TankCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TankCamera"));
	TankCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	TankCamera->bUsePawnControlRotation = false;
	// flat view
	TankCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
	TankCamera->OrthoWidth = 1024.f; // 1024.f by default
	TankCamera->AspectRatio = 3.f / 4.f;
	TankCamera->SetWorldRotation(FRotator(-90.f, 0.0f, 0.0f));

	// disable auto exposure
	TankCamera->PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
	TankCamera->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
	TankCamera->PostProcessSettings.AutoExposureMinBrightness = 1.f;
	TankCamera->PostProcessSettings.AutoExposureMaxBrightness = 1.f;

	//  disable motion blur
	TankCamera->PostProcessSettings.bOverride_MotionBlurAmount = true;
	TankCamera->PostProcessSettings.MotionBlurAmount = 0.f;

	/** tank body paper flipbook  */
	GetSprite()->SetupAttachment(RootComponent);
	GetSprite()->SetRelativeRotation(FRotator(0.f, 90.f, -90.f));
	GetSprite()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetSprite()->SetIsReplicated(true);

	/** projectile spawn point  */
	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(GetSprite());
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.f, 0.f, 20.f));
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

	/** capsule  */
	GetCapsuleComponent()->SetCapsuleHalfHeight(100.f);
	GetCapsuleComponent()->SetCapsuleRadius(13.f);
	GetCapsuleComponent()->SetCollisionProfileName("PlayerTank");

	/** character movement  */
	GetCharacterMovement()->GroundFriction = 1.f;
	GetCharacterMovement()->MaxWalkSpeed = 80.f;

	// Lock character motion onto the plane, so the character can't move in or out of the screen
	//GetCharacterMovement()->bConstrainToPlane = true;
	//GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, 0.0, -1.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::UseGlobalPhysicsSetting);

	/** Movement Audio Component  */
	MovementAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("Movement Audio"));
	MovementAudio->SetupAttachment(RootComponent);
	MovementAudio->bAutoActivate = true;
	MovementAudio->SetVolumeMultiplier(0.5);

	// Replication
	bReplicates = true;
	bReplicateMovement = true;

#pragma region Flipbooks

	/** set all tank flip books templates */

	// BeginPlay
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> BeginPlayFlipbookTemplate(TEXT("/Game/Spites/Flipbooks/Player_BeginPlay"));
	if (BeginPlayFlipbookTemplate.Succeeded())
	{
		BeginPlayFlipbook = BeginPlayFlipbookTemplate.Object;
	}

	// invulnerable
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> InvulnerableFlipbookTemplate(TEXT("/Game/Spites/Flipbooks/Player_invulnerable"));
	if (InvulnerableFlipbookTemplate.Succeeded())
	{
		InvulnerableFlipbook->SetFlipbook(InvulnerableFlipbookTemplate.Object);
	}

	// -------- IDLE -----------------------------------------------------------

	// Idle Star 0
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleStar0Template(TEXT("/Game/Spites/Flipbooks/Idle0"));
	if (IdleStar0Template.Succeeded())
	{
		IdleStar0 = IdleStar0Template.Object;
	}
	// Idle Star 1
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleStar1Template(TEXT("/Game/Spites/Flipbooks/Idle1"));
	if (IdleStar1Template.Succeeded())
	{
		IdleStar1 = IdleStar1Template.Object;
	}
	// Idle Star 2
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleStar2Template(TEXT("/Game/Spites/Flipbooks/Idle2"));
	if (IdleStar2Template.Succeeded())
	{
		IdleStar2 = IdleStar2Template.Object;
	}
	// Idle Star 3
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleStar3Template(TEXT("/Game/Spites/Flipbooks/Idle3"));
	if (IdleStar3Template.Succeeded())
	{
		IdleStar3 = IdleStar3Template.Object;
	}

	// -------- IDLE -----------------------------------------------------------


	// -------- MOVING -----------------------------------------------------------

	// moving star 0
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> MovingStar0Template(TEXT("/Game/Spites/Flipbooks/MovementStar0"));
	if (MovingStar0Template.Succeeded())
	{
		MovingStar0 = MovingStar0Template.Object;
	}

	// moving star 1
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> MovingStar1Template(TEXT("/Game/Spites/Flipbooks/MovementStar1"));
	if (MovingStar1Template.Succeeded())
	{
		MovingStar1 = MovingStar1Template.Object;
	}

	// moving star 2
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> MovingStar2Template(TEXT("/Game/Spites/Flipbooks/MovementStar2"));
	if (MovingStar2Template.Succeeded())
	{
		MovingStar2 = MovingStar2Template.Object;
	}

	// moving star 3
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> MovingStar3Template(TEXT("/Game/Spites/Flipbooks/MovementStar3"));
	if (MovingStar3Template.Succeeded())
	{
		MovingStar3 = MovingStar3Template.Object;
	}


	// -------- MOVING -----------------------------------------------------------

#pragma endregion

#pragma region Helpers

	/** set projectile slow BP template  */
	static ConstructorHelpers::FClassFinder<AProjectile> ProjectileSlowClass(TEXT("/Game/BP/Framework/BP_PlayerProjectileSlow"));
	if (ProjectileSlowClass.Class != NULL)
	{
		ProjectileSlowTemplate = ProjectileSlowClass.Class;
	}

	/** set projectile fast BP template  */
	static ConstructorHelpers::FClassFinder<AProjectile> ProjectileFastClass(TEXT("/Game/BP/Framework/BP_PlayerProjectileFast"));
	if (ProjectileFastClass.Class != NULL)
	{
		ProjectileFastTemplate = ProjectileFastClass.Class;
	}

	/** set FireSound   */
	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundTemplate(TEXT("/Game/Sounds/fire"));
	if (FireSoundTemplate.Succeeded())
	{
		FireSound = FireSoundTemplate.Object;
	}

#pragma endregion

	bMovingX = false;
	bInvulnerable = false;
}

void APlayerTank::BeginPlay()
{	
	Super::BeginPlay();

	/** save start location for further respawn after death  */
	StartLocation = GetActorLocation();
	
	/** overlap binding  */
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerTank::OnOverlapBegin);
	
	/** stores GM, PS, GS references  */
	SetReferences();
	
	/**  calls to simulate spawning  */
	StartSpawning();
}

void APlayerTank::SetReferences()
{
	/** set GM reference  */
	if (ABattleCityGameModeBase* TestBattleCityGameMode = Cast<ABattleCityGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		BattleCityGameMode = TestBattleCityGameMode;
	}

	/** set PS reference  */
	if (ABattleCityPlayerState* TestBattleCityPlayerState = Cast<ABattleCityPlayerState>(PlayerState))
	{
		BattleCityPlayerState = TestBattleCityPlayerState;
	}

	/** set GS reference  */
	if (ABattleCityGameState* TestBattleCityGameState = Cast<ABattleCityGameState>(GetWorld()->GetGameState()))
	{
		BattleCityGameState = TestBattleCityGameState;
	}

	if (!ensure(BattleCityGameMode)) { return; }
	if (!ensure(BattleCityPlayerState)) { return; }
	if (!ensure(BattleCityGameState)) { return; }
}

void APlayerTank::StartSpawning()
{
	GetSprite()->SetFlipbook(BeginPlayFlipbook);

	/** we have initial immortality !  */
	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);
		GetWorldTimerManager().SetTimer(TimerInvulnerable, this, &APlayerTank::StartPlay, InitialInvulnerableTime, false);
	}
}

void APlayerTank::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/** simple play tank engine sound when it is moving */
	PlayMoveSound();

	/** calls to determine current animation  */
	CalcAnimation();

	/** calls to update tank animation */
	UpdateAnimation();
}

void APlayerTank::IceSliding(bool bOnIce)
{
	/** ice sliding simulation   */
	if (bOnIce)
	{
		GetCharacterMovement()->MaxAcceleration = 50.f;
		GetCharacterMovement()->BrakingDecelerationWalking = 5.f;
	} 
	else /** revert to normal state  */
	{
		GetCharacterMovement()->MaxAcceleration = 2048.f;
		GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
	}
}

void APlayerTank::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// fire
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerTank::Fire);

	// movement
	PlayerInputComponent->BindAxis("MoveX", this, &APlayerTank::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &APlayerTank::MoveY);
}

void APlayerTank::AddStar()
{
	if (BattleCityPlayerState)
	{
		BattleCityPlayerState->Stars++;
		BattleCityPlayerState->Stars = FMath::Clamp(BattleCityPlayerState->Stars, 0, 3);
	}
}

void APlayerTank::NullifyProjectileReference(class AProjectile* ProjectileToNullify)
{
	if (ProjectileToNullify == Projectile01)
	{
		Projectile01 = nullptr;
	}
	else if (ProjectileToNullify == Projectile02)
	{
		Projectile02 = nullptr;
	}
}

float APlayerTank::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!ensure(BattleCityPlayerState)) { return 0.f; }

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		if (!bInvulnerable)
		{
			int32& Stars = BattleCityPlayerState->Stars;
			Stars--;
			if (Stars < 1)
			{
				/** prepare to respawn tank  */
				Stars = 0;

				/** decrease lives  */
				if (BattleCityGameMode && BattleCityPlayerState)
				{
					BattleCityPlayerState->Lives--;

					if (BattleCityPlayerState->Lives < 0)
					{
						/** hide tank  */
						GetSprite()->SetVisibility(false);
						InvulnerableFlipbook->SetVisibility(false);
						GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

						/** game over  */
						BattleCityGameMode->GameOver();
					}
					else
					{
						GetWorld()->SpawnActor<AExplodeFX>(AExplodeFX::StaticClass(), GetActorTransform());
						SetActorLocation(StartLocation);
						SetInvulnerable(InitialInvulnerableTime);
					}
				}
			}
		}
	}

	return ActualDamage;
}

void APlayerTank::AddTankKilled() const
{
	if (BattleCityPlayerState)
	{
		BattleCityPlayerState->TanksKilled++;
	}
}

void APlayerTank::MoveX(float Value)
{
	if (Value != 0.f)
	{
		MovementVector = FVector(1.f, 0.f, 0.f);

		AddMovementInput(MovementVector, Value);

		RotateTank(Value);

		bMovingX = true;

		return;
	}

	bMovingX = false;
}

void APlayerTank::MoveY(float Value)
{
	if (!bMovingX && Value != 0.f)
	{
		MovementVector = FVector(0.f, 1.f, 0.f);

		AddMovementInput(MovementVector, Value);
		
		RotateTank(Value);
	}
}

void APlayerTank::Fire()
{
	if (!ensure(ProjectileSlowTemplate)) { return; }
	if (!ensure(ProjectileFastTemplate)) { return; }
	if (!ensure(BattleCityPlayerState)) { return; }

	UWorld* const World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
		FRotator SpawnRotation = GetCapsuleComponent()->GetComponentRotation();

		int32 Stars = BattleCityPlayerState->Stars;
		
		if (Projectile01 == nullptr)
		{
			/** one slow */
			if (Stars == 0)
			{
				/**   */
				if (AProjectile* Spawned = World->SpawnActor<AProjectile>(ProjectileSlowTemplate, SpawnLocation, SpawnRotation, SpawnParams))
				{
					Projectile01 = Spawned;
					// try and play the sound if specified
					if (FireSound != NULL)
					{
						UGameplayStatics::PlaySound2D(this, FireSound);
					}
				}
			} /** one fast  */
			else if (Stars == 1)
			{
				if (AProjectile* Spawned = World->SpawnActor<AProjectile>(ProjectileFastTemplate, SpawnLocation, SpawnRotation, SpawnParams))
				{
					Projectile01 = Spawned;
					// try and play the sound if specified
					if (FireSound != NULL)
					{
						UGameplayStatics::PlaySound2D(this, FireSound);
					}
				}
			}
		}

		if (Projectile01 == nullptr || Projectile02 == nullptr)
		{
			/** two fast  */
			if (Stars >= 2)
			{
				if (AProjectile* Spawned = World->SpawnActor<AProjectile>(ProjectileFastTemplate, SpawnLocation, SpawnRotation, SpawnParams))
				{
					// try and play the sound if specified
					if (FireSound != NULL)
					{
						UGameplayStatics::PlaySound2D(this, FireSound);
					}
					if (Projectile01 == nullptr)
					{
						Projectile01 = Spawned;
					}
					else
					{
						Projectile02 = Spawned;
					}

					/** piercing  */
					if (Stars == 3)
					{
						Spawned->SetPiercing(true);
					}
				}
			}
		}
	}
}

void APlayerTank::RotateTank(float Value)
{
	if (Controller)
	{
		/** make desired rotator from movement vector  */
		FRotator RotatorFromMovementVector = FRotationMatrix::MakeFromX(MovementVector * Value).Rotator();

		/** Rotate in desired direction  */
		Controller->SetControlRotation(RotatorFromMovementVector);
	}
}

void APlayerTank::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself. 
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if(APickupBase* PickupBase = Cast<APickupBase>(OtherActor))
		{
			PickupBase->OnPickup(this);
		}
	}
}

void APlayerTank::CalcAnimation()
{
	if (TankAnimState == ETankAnimState::BeginingPlay)
	{
		return;
	}

	float Velocity = GetCharacterMovement()->Velocity.Size();
	int32 Stars = BattleCityPlayerState->Stars;

	/** idle  */
	if (Velocity == 0.f)
	{
		switch (Stars)
		{
		case 0:
			SetTankAnimState(ETankAnimState::Idle_Lvl_00);
			break;
		case 1:
			SetTankAnimState(ETankAnimState::Idle_Lvl_01);
			break;
		case 2:
			SetTankAnimState(ETankAnimState::Idle_Lvl_02);
			break;
		case 3:
			SetTankAnimState(ETankAnimState::Idle_Lvl_03);
			break;
		default:
			break;
		}
	}
	else /** moving  */
	{
		switch (Stars)
		{
		case 0:
			SetTankAnimState(ETankAnimState::Moving_Lvl_00);
			break;
		case 1:
			SetTankAnimState(ETankAnimState::Moving_Lvl_01);
			break;
		case 2:
			SetTankAnimState(ETankAnimState::Moving_Lvl_02);
			break;
		case 3:
			SetTankAnimState(ETankAnimState::Moving_Lvl_03);
			break;
		default:
			break;
		}
	}
}

void APlayerTank::PlayMoveSound()
{
	float Velocity = GetCharacterMovement()->Velocity.Size();
	if (Velocity > 1.f)
	{
		MovementAudio->SetVolumeMultiplier(0.5f);
	} 
	else
	{
		MovementAudio->SetVolumeMultiplier(0.f);
	}
}

void APlayerTank::UpdateAnimation()
{
	switch (TankAnimState)
	{
	case ETankAnimState::Idle_Lvl_00:
		GetSprite()->SetFlipbook(IdleStar0);
		break;
	case ETankAnimState::Idle_Lvl_01:
		GetSprite()->SetFlipbook(IdleStar1);
		break;
	case ETankAnimState::Idle_Lvl_02:
		GetSprite()->SetFlipbook(IdleStar2);
		break;
	case ETankAnimState::Idle_Lvl_03:
		GetSprite()->SetFlipbook(IdleStar3);
		break;

	case ETankAnimState::Moving_Lvl_00:
		GetSprite()->SetFlipbook(MovingStar0);
		break;
	case ETankAnimState::Moving_Lvl_01:
		GetSprite()->SetFlipbook(MovingStar1);
		break;
	case ETankAnimState::Moving_Lvl_02:
		GetSprite()->SetFlipbook(MovingStar2);
		break;
	case ETankAnimState::Moving_Lvl_03:
		GetSprite()->SetFlipbook(MovingStar3);
		break;
		
	default:
		break;	
	}
}

void APlayerTank::StartPlay()
{
	EnableInput(PlayerController);
	SetInvulnerable(InitialInvulnerableTime);
	SetTankAnimState(ETankAnimState::Idle_Lvl_00);
}

void APlayerTank::SetInvulnerable(float InvulnerableTime)
{
	bInvulnerable = true;

	InvulnerableFlipbook->SetVisibility(bInvulnerable);

	GetWorldTimerManager().SetTimer(TimerInvulnerable, this, &APlayerTank::DisableInvulnerable, InvulnerableTime, false);
}

void APlayerTank::DisableInvulnerable()
{
	bInvulnerable = false;

	InvulnerableFlipbook->SetVisibility(bInvulnerable);

	GetWorldTimerManager().ClearTimer(TimerInvulnerable);
}
