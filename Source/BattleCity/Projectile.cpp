// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "ConstructorHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "ExplodeFX.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	/** ROOT  */
	ROOT = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
	SetRootComponent(ROOT);

	/** projectile sprite */
	GetRenderComponent()->SetupAttachment(RootComponent);
	GetRenderComponent()->SetRelativeRotation(FRotator(0.f, 90.f, -90.f));
	GetRenderComponent()->SetCollisionProfileName("PlayerProjectilePresets");
	GetRenderComponent()->SetMobility(EComponentMobility::Movable);

	/** set projectile sprite template */
	static ConstructorHelpers::FObjectFinder<UPaperSprite> PaperSpriteTemplate(TEXT("/Game/Spites/Source/NewFolder/Projectile"));
	if (PaperSpriteTemplate.Succeeded())
	{
		GetRenderComponent()->SetSprite(PaperSpriteTemplate.Object);
	}

	/** Projectile Movement  */
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->MaxSpeed = 130.f;
	ProjectileMovementComponent->InitialSpeed = 130.f;
	
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	/** 3 stars to collect to enable piercing  */
	bPiercing = false;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetRenderComponent()->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/** TODO make base tank class  */
	if(APlayerTank* PlayerTank = Cast<APlayerTank>(GetOwner()))
	{
		PlayerTank->NullifyProjectileReference(this);
	}

	if (AEnemyTank* EnemyTank = Cast<AEnemyTank>(GetOwner()))
	{
		EnemyTank->NullifyProjectileReference(this);
	}
}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		/** try to inflict damage to everyone
		* Every actor that intended to interact with this projectile 
		* must handle this logic overriding TakeDamage() function
		*/
		OtherActor->TakeDamage(DamageToCause, FDamageEvent(), nullptr, this);
		
		/** spawn Explode FX  */
		FTransform SpawnTransform = OtherActor->GetActorTransform();
		SpawnTransform.SetLocation(SweepResult.Location);
		GetWorld()->SpawnActor<AExplodeFX>(AExplodeFX::StaticClass(), GetActorTransform());
		
		Destroy();
	}
}

