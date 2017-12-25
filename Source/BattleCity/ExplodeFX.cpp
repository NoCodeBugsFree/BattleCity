// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplodeFX.h"
#include "PaperFlipbookComponent.h"
#include "ConstructorHelpers.h"
#include "PaperFlipbook.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AExplodeFX::AExplodeFX()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ExplodeFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbookComponent"));
	ExplodeFlipbook->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExplodeFlipbook->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));
	SetRootComponent(ExplodeFlipbook);

	/** set flipbook  */
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> ExplodeFlipbookTemplate(TEXT("/Game/Spites/Flipbooks/Explode"));
	if (ExplodeFlipbookTemplate.Succeeded())
	{
		ExplodeFlipbook->SetFlipbook(ExplodeFlipbookTemplate.Object);
	}

	/** set ExplodeSound   */
	static ConstructorHelpers::FObjectFinder<USoundBase> ExplodeSoundTemplate(TEXT("/Game/Sounds/explosion"));
	if (ExplodeSoundTemplate.Succeeded())
	{
		ExplodeSound = ExplodeSoundTemplate.Object;
	}

}

// Called when the game starts or when spawned
void AExplodeFX::BeginPlay()
{
	Super::BeginPlay();

	// try and play the sound if specified
	if (ExplodeSound != NULL)
	{
		UGameplayStatics::PlaySound2D(this, ExplodeSound, 0.5f);
	}
	ExplodeFlipbook->SetLooping(false);
	ExplodeFlipbook->OnFinishedPlaying.AddDynamic(this, &AExplodeFX::DestroySelf);
	
}

void AExplodeFX::DestroySelf()
{
	Destroy();
}


