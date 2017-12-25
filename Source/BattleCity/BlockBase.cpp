// Fill out your copyright notice in the Description page of Project Settings.

#include "BlockBase.h"
#include "Components/ChildActorComponent.h"


// Sets default values
ABlockBase::ABlockBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/** Root  */
	ROOT = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
	SetRootComponent(ROOT);

	/** pieces  */
	Piece01 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Piece01"));
	Piece01->SetupAttachment(ROOT);
	Piece01->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	Piece01->SetRelativeRotation(FRotator(0.f, -180.f, -90.f));
	Piece01->SetMobility(EComponentMobility::Static);

	Piece02 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Piece02"));
	Piece02->SetupAttachment(ROOT);
	Piece02->SetRelativeLocation(FVector(16.f, 0.f, 50.f));
	Piece02->SetRelativeRotation(FRotator(0.f, -180.f, -90.f));
	Piece02->SetMobility(EComponentMobility::Static);

	Piece03 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Piece03"));
	Piece03->SetupAttachment(ROOT);
	Piece03->SetRelativeLocation(FVector(16.f, 16.f, 50.f));
	Piece03->SetRelativeRotation(FRotator(0.f, -180.f, -90.f));
	Piece03->SetMobility(EComponentMobility::Static);

	Piece04 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Piece04"));
	Piece04->SetupAttachment(ROOT);
	Piece04->SetRelativeLocation(FVector(0.f, 16.f, 50.f));
	Piece04->SetRelativeRotation(FRotator(0.f, -180.f, -90.f));
	Piece04->SetMobility(EComponentMobility::Static);

}