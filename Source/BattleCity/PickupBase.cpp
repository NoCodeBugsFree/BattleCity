// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupBase.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"
#include "PlayerTank.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"

APickupBase::APickupBase()
{
	/** root  */
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRootComponent);

	/** flip book  */
	GetRenderComponent()->SetCollisionProfileName("BasePickup");
	GetRenderComponent()->SetupAttachment(RootComponent);
	GetRenderComponent()->SetRelativeLocation(FVector(0.f, 0.f, 51.f));
	GetRenderComponent()->SetRelativeRotation(FRotator(0.f, 90.f, -90.f));

	/** set pickup sound   */
	static ConstructorHelpers::FObjectFinder<USoundBase> PickupSoundTemplate(TEXT("/Game/Sounds/bonus"));
	if (PickupSoundTemplate.Succeeded())
	{
		PickupSound = PickupSoundTemplate.Object;
	}

	InitialLifeSpan = 10.f;
}

void APickupBase::OnPickup_Implementation(APlayerTank* PlayerTank)
{
	// try and play the sound if specified
	if (PickupSound != NULL)
	{
		UGameplayStatics::PlaySound2D(this, PickupSound);
	}
	Destroy();
}
