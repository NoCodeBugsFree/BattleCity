// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerBase.h"
#include "Components/ChildActorComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "ConstructorHelpers.h"
#include "PieceBase.h"
#include "BattleCityGameModeBase.h"
#include "Projectile.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/**	scene root   */
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneComponent);

	/** Base Sprite  */
	BaseSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BaseSprite"));
	BaseSprite->SetupAttachment(RootComponent);
	BaseSprite->SetCollisionProfileName("BlockAll");
	BaseSprite->SetMobility(EComponentMobility::Stationary);

	/** set sprite  */
	static ConstructorHelpers::FObjectFinder<UPaperSprite> PaperSpriteTemplate(TEXT("/Game/Spites/Flipbooks/Pickups/BaseAlive"));
	if (PaperSpriteTemplate.Succeeded())
	{
		BaseSprite->SetSprite(PaperSpriteTemplate.Object);
	}
}

void APlayerBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	/** Fill the bricks array  */
	AllBricks.Empty();

	for (UActorComponent* Child : GetComponentsByClass(UChildActorComponent::StaticClass()))
	{
		if(UChildActorComponent* ChildActorComponent = Cast<UChildActorComponent>(Child))
		{
			APieceBase* PieceBase = Cast<APieceBase>(ChildActorComponent->GetChildActor());
			if (PieceBase)
			{
				AllBricks.Push(PieceBase);
			}
		}
	}
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
	/** on kill box overlap  */
	BaseSprite->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::OnBaseCenterOverlapBegin);

	/** store our ref in game mode  */
	if (ABattleCityGameModeBase* BattleCityGameMode = Cast<ABattleCityGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		BattleCityGameMode->SetPlayerBaseRef(this);
	}
}

void APlayerBase::OnBaseCenterOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself. 
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		/** if we hit by a projectile  */
		if(AProjectile* Projectile = Cast<AProjectile>(OtherActor))
		{
			/**  no more collision */
			BaseSprite->OnComponentBeginOverlap.RemoveDynamic(this, &APlayerBase::OnBaseCenterOverlapBegin);
			BaseSprite->SetCollisionProfileName("NoCollision");

			BaseSprite->SetSprite(BaseDead);


			
			// 1 sec delay to game over
			FTimerDelegate GameOverDelegate;
			GameOverDelegate.BindLambda( [&] { 
				if (ABattleCityGameModeBase* BattleCityGameMode = Cast<ABattleCityGameModeBase>(GetWorld()->GetAuthGameMode()))
				{
					BattleCityGameMode->GameOver();
				}
			});
			
			FTimerHandle GameOverTimer;
			GetWorldTimerManager().SetTimer(GameOverTimer, GameOverDelegate, 1.f, false);
		}
	}
}

void APlayerBase::SetInvulnerable()
{
	/** change each brick  */
	for (APieceBase* PieceBase : AllBricks)
	{
		if (PieceBase)
		{
			PieceBase->SetPieceType(EPieceType::Steel);
			PieceBase->GetPaperSpriteComponent()->SetSprite(SteelSprite);

			/** regenerate missed blocks  */
			PieceBase->GetRenderComponent()->SetVisibility(true);
			PieceBase->GetRenderComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}

	// Bind Lambda to revert base state to vulnerable
	FTimerDelegate SetVulnurableDelegate;
	SetVulnurableDelegate.BindLambda([&] {
		for (APieceBase* PieceBase : AllBricks)
		{
			if (PieceBase)
			{
				PieceBase->SetPieceType(EPieceType::Brick);
				PieceBase->GetPaperSpriteComponent()->SetSprite(BrickSprite);

				/** regenerate missed blocks  */
				PieceBase->GetRenderComponent()->SetVisibility(true);
				PieceBase->GetRenderComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
		}
	});

	FTimerHandle VulnurableTimer;
	GetWorldTimerManager().SetTimer(VulnurableTimer, SetVulnurableDelegate, InvulnerableTime, false);
}