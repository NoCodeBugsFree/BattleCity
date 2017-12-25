// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawner.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "PickupBase.h"
#include "EnemyTank.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "BattleCityGameState.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/** Root  */
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
	SetRootComponent(SceneComponent);

	/** level camera  */
	LevelCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TankCamera"));
	LevelCamera->SetupAttachment(RootComponent);
	LevelCamera->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	LevelCamera->bUsePawnControlRotation = false;
	// flat view
	LevelCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
	LevelCamera->OrthoWidth = 750.f; // 1024.f by default
	LevelCamera->AspectRatio = 16.f / 9.f;
	LevelCamera->SetWorldRotation(FRotator(-90.f, 0.0f, 0.0f));

	// disable auto exposure
	LevelCamera->PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
	LevelCamera->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
	LevelCamera->PostProcessSettings.AutoExposureMinBrightness = 1.f;
	LevelCamera->PostProcessSettings.AutoExposureMaxBrightness = 1.f;

	//  disable motion blur
	LevelCamera->PostProcessSettings.bOverride_MotionBlurAmount = true;
	LevelCamera->PostProcessSettings.MotionBlurAmount = 0.f;
	
	/* box that covers the whole map to spawn pickups  */
	PickupSpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupSpawningBox"));
	PickupSpawningBox->SetupAttachment(RootComponent);
	PickupSpawningBox->SetCollisionProfileName("NoCollision");
	PickupSpawningBox->SetBoxExtent(FVector(200.f, 200.f, 50.f));
	
	/** black  background sprite */
	BlackBackGroundSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BlackBackGroundSprite"));
	BlackBackGroundSprite->SetupAttachment(RootComponent);
	BlackBackGroundSprite->SetRelativeLocation(FVector(0.f, 0.f, -10.f));
	BlackBackGroundSprite->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
	BlackBackGroundSprite->SetRelativeScale3D(FVector(26.f, 0.f, 26.f));

	/** gray background sprite */
	GrayBackGroundSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("GrayBackGroundSprite"));
	GrayBackGroundSprite->SetupAttachment(RootComponent);
	GrayBackGroundSprite->SetRelativeLocation(FVector(0.f, 0.f, -20.f));
	GrayBackGroundSprite->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
	GrayBackGroundSprite->SetRelativeScale3D(FVector(150.f, 0.f, 150.f));

	/** set black sprite asset ref */
	static ConstructorHelpers::FObjectFinder<UPaperSprite> BlackBackGroundSpriteTemplate(TEXT("/Game/Spites/LevelGeometry/PS_Black"));
	if (BlackBackGroundSpriteTemplate.Succeeded())
	{
		BlackBackGroundSprite->SetSprite(BlackBackGroundSpriteTemplate.Object);
	}

	/** set gray sprite asset ref */
	static ConstructorHelpers::FObjectFinder<UPaperSprite> GrayBackGroundSpriteTemplate(TEXT("/Game/Spites/LevelGeometry/PS_Gray"));
	if (GrayBackGroundSpriteTemplate.Succeeded())
	{
		GrayBackGroundSprite->SetSprite(GrayBackGroundSpriteTemplate.Object);
	}
	
	/* 1st enemy spawn point  */
	EnemySpawnPoint01 = CreateDefaultSubobject<UArrowComponent>(TEXT("EnemySpawnPoint01"));
	EnemySpawnPoint01->SetupAttachment(RootComponent);

	/* 2nd enemy spawn point  */
	EnemySpawnPoint02 = CreateDefaultSubobject<UArrowComponent>(TEXT("EnemySpawnPoint02"));
	EnemySpawnPoint02->SetupAttachment(RootComponent);

	/* 3rd enemy spawn point  */
	EnemySpawningPoint3 = CreateDefaultSubobject<UArrowComponent>(TEXT("EnemySpawningPoint3"));
	EnemySpawningPoint3->SetupAttachment(RootComponent);

	/* 1st blocking volume */
	BlockingBox01 = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBox01"));
	BlockingBox01->SetupAttachment(RootComponent);
	BlockingBox01->SetCollisionProfileName("BlockAll");
	BlockingBox01->SetBoxExtent(FVector(250.f, 20.f, 100.f));

	/* 2ns blocking volume */
	BlockingBox02 = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBox02"));
	BlockingBox02->SetupAttachment(RootComponent);
	BlockingBox02->SetCollisionProfileName("BlockAll");
	BlockingBox02->SetBoxExtent(FVector(250.f, 20.f, 100.f));

	/* 3rd blocking volume */
	BlockingBox03 = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBox03"));
	BlockingBox03->SetupAttachment(RootComponent);
	BlockingBox03->SetCollisionProfileName("BlockAll");
	BlockingBox03->SetBoxExtent(FVector(250.f, 20.f, 100.f));

	/* 4th blocking volume */
	BlockingBox04 = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBox04"));
	BlockingBox04->SetupAttachment(RootComponent);
	BlockingBox04->SetCollisionProfileName("BlockAll");
	BlockingBox04->SetBoxExtent(FVector(250.f, 20.f, 100.f));

#pragma region TankTemplates

	// -------- Tank templates -------------------------------------------------------

	static ConstructorHelpers::FClassFinder<AEnemyTank> BaseTankClass(TEXT("/Game/BP/Enemy/BP_EnemyTankBase"));
	if (BaseTankClass.Class != NULL)
	{
		BaseTankTemplate = BaseTankClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AEnemyTank> FastTankClass(TEXT("/Game/BP/Enemy/BP_EnemyTankFast"));
	if (FastTankClass.Class != NULL)
	{
		FastTankTemplate = FastTankClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AEnemyTank> PowerTankClass(TEXT("/Game/BP/Enemy/BP_EnemyTankHeavy"));
	if (PowerTankClass.Class != NULL)
	{
		PowerTankTemplate = PowerTankClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AEnemyTank> HeavyTankClass(TEXT("/Game/BP/Enemy/BP_EnemyTankPower"));
	if (HeavyTankClass.Class != NULL)
	{
		HeavyTankTemplate = HeavyTankClass.Class;
	}


	// -------- Tank templates -------------------------------------------------------

#pragma endregion

	bRandomEnemies = false;
	bUseSpawnerCamera = true;
}

void ASpawner::BeginPlay()
{
	/** fill the PickupTypes and EnemyTypes in BP */
	Super::BeginPlay();

	if(ABattleCityGameState* TestBattleCityGameState = Cast<ABattleCityGameState>(GetWorld()->GetGameState()))
	{
		BattleCityGameState = TestBattleCityGameState;
	}

	if (!ensure(BattleCityGameState)) { return; }

	/**  fill the pickups in BP */
	if (!ensure(PickupTypes.Num())) { return; }

	/** fill the enemies list in BP  */
	if (!ensure(EnemiesToSpawn.Num())) { return; }

	/**  TODO server */
	if (bUseSpawnerCamera)
	{
		// for each player controller use a level camera instead debug player camera
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; It++)
		{
			if (APlayerController* PlayerController = Cast<APlayerController>(*It))
			{
				PlayerController->SetViewTarget(this);
			}
		}
	}

	/** set spawn enemy timer */
	FTimerHandle SpawnTimer;
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawner::SpawnEnemy, EnemySpawnDelay, true);
}

void ASpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bRandomEnemies)
	{
		EnemiesToSpawn.Empty();

		for (int32 iCount = 0; iCount < EnemiesAmount; iCount++)
		{
			int32 RandInt = FMath::RandRange(0, 3); // TODO remove magic number

			switch (RandInt)
			{
			case 0:
				if (!ensure(BaseTankTemplate)) { return; }
				EnemiesToSpawn.Add(BaseTankTemplate);
				break;

			case 1:
				if (!ensure(FastTankTemplate)) { return; }
				EnemiesToSpawn.Add(FastTankTemplate);
				break;

			case 2:
				if (!ensure(PowerTankTemplate)) { return; }
				EnemiesToSpawn.Add(PowerTankTemplate);
				break;

			case 3:
				if (!ensure(HeavyTankTemplate)) { return; }
				EnemiesToSpawn.Add(HeavyTankTemplate);
				break;

			default:
				break;
						
			}
		}
	}
}

void ASpawner::SpawnEnemy()
{
	/** random int32 for spawning box   */
	int32 RandInt = FMath::RandRange(0, 2);
		
	/** find current spawn transform  */
	FTransform SpawnTransform;
	switch (RandInt)
	{
	case 0:
		SpawnTransform = EnemySpawnPoint01->GetComponentTransform();
		break;
	case 1:
		SpawnTransform = EnemySpawnPoint02->GetComponentTransform();
		break;
	case 2:
		SpawnTransform = EnemySpawningPoint3->GetComponentTransform();
		break;			
	}
		
	if (BattleCityGameState->EnemiesRemaining > 0 && EnemiesOnField.Num() < MaxEnemiesOnField)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			if (EnemiesToSpawn.IsValidIndex(0) && EnemiesToSpawn[0])
			{
				if (AActor* EnemyTank = World->SpawnActor<AActor>(EnemiesToSpawn[0], SpawnTransform, SpawnParams))
				{
					EnemiesOnField.AddUnique(EnemyTank);
					EnemiesToSpawn.RemoveAt(0);
					BattleCityGameState->EnemiesRemaining--;
				}
			}
		}
	}
}

void ASpawner::SpawnPickup()
{
	/** random int32 for pickup type   */
	int32 RandInt = FMath::RandRange(0, PickupTypes.Num() - 1);
		
	UWorld* const World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;

		FVector SpawnOrigin = PickupSpawningBox->Bounds.Origin;
		FVector SpawnExtent = PickupSpawningBox->Bounds.BoxExtent;
		FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
		SpawnLocation.Z = PickupSpawningBox->GetComponentLocation().Z;
		FRotator SpawnRotation = FRotator::ZeroRotator;
			
		if (PickupTypes.IsValidIndex(RandInt) && PickupTypes[RandInt])
		{
			APickupBase* Spawned = World->SpawnActor<APickupBase>(PickupTypes[RandInt], SpawnLocation, SpawnRotation, SpawnParams);
		}
	}
}

void ASpawner::RemoveEnemy(AActor* TankToRemove)
{
	if (TankToRemove && EnemiesOnField.Contains(TankToRemove))
	{
		EnemiesOnField.Remove(TankToRemove);
	}
}
