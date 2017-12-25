// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyTank.h"
#include "PaperFlipbookComponent.h"
#include "ConstructorHelpers.h"
#include "PaperFlipbook.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BattleCityGameModeBase.h"
#include "Projectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "ExplodeFX.h"
#include "PlayerTank.h"
#include "Spawner.h"

AEnemyTank::AEnemyTank()
{
	/**   */
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetBoxExtent(FVector(13.f, 13.f, 100.f));
	BoxComponent->SetCollisionProfileName("EnemyTank");

	/** Paper Flipbook Component  */
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
	GetCapsuleComponent()->SetCollisionProfileName("EnemyTank");

	/** character movement  */
	GetCharacterMovement()->GroundFriction = 1.f;
	GetCharacterMovement()->MaxWalkSpeed = 80.f;
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::UseGlobalPhysicsSetting);

	bHasBonus = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Replication
	bReplicates = true;
	bReplicateMovement = true;

#pragma region Helpers

	// BeginPlay
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> BeginPlayFlipbookTemplate(TEXT("/Game/Spites/Flipbooks/Player_BeginPlay"));
	if (BeginPlayFlipbookTemplate.Succeeded())
	{
		BeginPlayFlipbook = BeginPlayFlipbookTemplate.Object;
	}

	// moving
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> MovingTemplate(TEXT("/Game/Spites/Flipbooks/Enemies/EnemyBase"));
	if (MovingTemplate.Succeeded())
	{
		MovingFlipbook = MovingTemplate.Object;
	}

	// idle
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook>IdleTemplate(TEXT("/Game/Spites/Flipbooks/Enemies/EnemyBaseIdle"));
	if (IdleTemplate.Succeeded())
	{
		FrozenFlipbook = IdleTemplate.Object;
	}

	/** set projectile slow BP template  */
	static ConstructorHelpers::FClassFinder<AProjectile> ProjectileSlowClass(TEXT("/Game/BP/Framework/BP_EnemyProjectileSlow"));
	if (ProjectileSlowClass.Class != NULL)
	{
		ProjectileTemplate = ProjectileSlowClass.Class;
	}

	/** set FireSound   */
	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundTemplate(TEXT("/Game/Sounds/fire"));
	if (FireSoundTemplate.Succeeded())
	{
		FireSound = FireSoundTemplate.Object;
	}

#pragma endregion

}

void AEnemyTank::BeginPlay()  
{
	Super::BeginPlay();

	SetTankAnimState(EEnemyTankAnimState::BeginingPlay);

	/** set random bonus state  */
	InitBonusState();
	
	/** set game mode ref  */
	if (ABattleCityGameModeBase* TestBattleCityGameMode = Cast<ABattleCityGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		BattleCityGameMode = TestBattleCityGameMode;
	}

	/**  delay TankSpawningTime sec and go to moving state  */
	FTimerDelegate TimerDelegate;
	FTimerHandle SpawnDelayTimer;
	TimerDelegate.BindUFunction(this, FName("SetTankAnimState"), EEnemyTankAnimState::Moving);
	GetWorldTimerManager().SetTimer(SpawnDelayTimer, TimerDelegate, TankSpawningTime, false);

	/** fire if can per timer tick  */
	FTimerHandle FireTimer;
	GetWorldTimerManager().SetTimer(FireTimer, this, &AEnemyTank::Fire, FireDelay, true);

	/**  random rotate per timer tick */
	FTimerHandle RotateTankTimer;
	GetWorldTimerManager().SetTimer(RotateTankTimer, this, &AEnemyTank::RandomRotateTank, FMath::RandRange(RotationTimeMin, RotationTimeMax), true);
}

void AEnemyTank::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/** move forever  */
	if (TankAnimState == EEnemyTankAnimState::Moving)
	{
		AddMovementInput(GetActorForwardVector(), 1.f);
	}

	/** animate color if has bonus  */
	AnimateColor(DeltaSeconds);
}

void AEnemyTank::AnimateColor(float DeltaSeconds)
{
	if (bHasBonus)
	{
		float Alpha = FMath::Abs(FMath::Sin(DeltaSeconds * BonusAnimSpeed));

		FLinearColor ColorToSet = UKismetMathLibrary::LinearColorLerp(FLinearColor::White, FLinearColor::Red, Alpha);

		GetSprite()->SetSpriteColor(ColorToSet);
	}
}

void AEnemyTank::Fire()
{
	/** fire only if we moving and have no active projectile on the field  */
	if (TankAnimState == EEnemyTankAnimState::Moving && Projectile == nullptr)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
			FRotator SpawnRotation = GetCapsuleComponent()->GetComponentRotation();

			if (ProjectileTemplate)
			{
				/** fire  */
				if (AProjectile* Spawned = World->SpawnActor<AProjectile>(ProjectileTemplate, SpawnLocation, SpawnRotation, SpawnParams))
				{
					Projectile = Spawned;

					// try and play the sound if specified
					if (FireSound != NULL)
					{
						UGameplayStatics::PlaySound2D(this, FireSound, 0.25f);
					}
				}
			}
		}
	}
}

void AEnemyTank::RandomRotateTank()
{
	if (TankAnimState == EEnemyTankAnimState::Moving)
	{
		int32 RandomInt = FMath::RandRange(0, 3);
		FRotator RandomRotation;
		switch (RandomInt)
		{
		case 0:
			RandomRotation = FRotator(0.f, 90.f, 0.f);
			break;

		case 1:
			RandomRotation = FRotator(0.f, -90.f, 0.f);
			break;

		case 2:
			RandomRotation = FRotator(0.f, 180.f, 0.f);
			break;

		case 3:
			RandomRotation = FRotator(0.f, 0.f, 0.f);
			break;

		default:

			break;
		}

		SetActorRotation(RandomRotation);
	}
}

void AEnemyTank::Freeze_Implementation(bool bNewFrozen)
{
	if (bNewFrozen)
	{
		SetTankAnimState(EEnemyTankAnimState::Frozen);
	} 
	else
	{
		SetTankAnimState(EEnemyTankAnimState::Moving);
	}
}

void AEnemyTank::NullifyProjectileReference(class AProjectile* ProjectileToNullify)
{
	if (ProjectileToNullify == Projectile)
	{
		Projectile = nullptr;
	}
}

float AEnemyTank::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (TankAnimState != EEnemyTankAnimState::BeginingPlay)
	{
		if (bHasBonus && BattleCityGameMode)
		{
			/** one bonus per tank  */
			bHasBonus = false;
			BattleCityGameMode->SpawnBonus();
		}

		Health -= ActualDamage;
		if (Health <= 0) // we are dead
		{
			/** spawn explode FX  */
			FTransform SpawnTransform = GetActorTransform();
			SpawnTransform.SetScale3D(FVector(1.5f, 1.5f, 1.5f));
			GetWorld()->SpawnActor<AExplodeFX>(AExplodeFX::StaticClass(), SpawnTransform);

			/** add our killer's score  */
			if (DamageCauser)
			{
				if (APlayerTank* PlayerTank = Cast<APlayerTank>(DamageCauser->GetOwner()))
				{
					PlayerTank->AddTankKilled();
				}
			}
			else if(EventInstigator)
			{
				if (APlayerTank* PlayerTank = Cast<APlayerTank>(EventInstigator->GetPawn()))
				{
					PlayerTank->AddTankKilled();
				}
			}

			/** remove ourself from owner (Spawner)  Enemies array */
			if (ASpawner* Spawner = Cast<ASpawner>(GetOwner()))
			{
				Spawner->RemoveEnemy(this);
			}

			BattleCityGameMode->CheckForNextLevel();

			Destroy();
		}
	}
	return ActualDamage;
}

void AEnemyTank::SetTankAnimState(EEnemyTankAnimState NewState)
{
	TankAnimState = NewState;

	/** set according flipbook  */
	switch (TankAnimState)
	{
	case EEnemyTankAnimState::BeginingPlay:
		GetSprite()->SetFlipbook(BeginPlayFlipbook);
		break;
	
	case EEnemyTankAnimState::Frozen:
		GetSprite()->SetFlipbook(FrozenFlipbook);
		break;

	case EEnemyTankAnimState::Moving:
		GetSprite()->SetFlipbook(MovingFlipbook);
		break;

	default:

		break;	
	}
}

void AEnemyTank::InitBonusState()
{
	int32 RandomInt = FMath::RandRange(0, 99);
	if (RandomInt < BonusProbability)
	{
		bHasBonus = true;
	}
}
