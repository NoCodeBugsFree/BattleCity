// Fill out your copyright notice in the Description page of Project Settings.

#include "PieceBase.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "ConstructorHelpers.h"

// Sets default values
APieceBase::APieceBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetRenderComponent()->SetMobility(EComponentMobility::Stationary);

#pragma region Sprites

	static ConstructorHelpers::FObjectFinder<UPaperSprite> BrickTemplate(TEXT("/Game/Spites/LevelGeometry/PS_Brick"));
	if (BrickTemplate.Succeeded())
	{
		Brick = BrickTemplate.Object;
	}

	static ConstructorHelpers::FObjectFinder<UPaperSprite> SteelTemplate(TEXT("/Game/Spites/LevelGeometry/PS_Steel"));
	if (SteelTemplate.Succeeded())
	{
		Steel = SteelTemplate.Object;
	}

	static ConstructorHelpers::FObjectFinder<UPaperSprite> IceTemplate(TEXT("/Game/Spites/LevelGeometry/PS_Ice"));
	if (IceTemplate.Succeeded())
	{
		Ice = IceTemplate.Object;
	}

	static ConstructorHelpers::FObjectFinder<UPaperSprite> WaterTemplate(TEXT("/Game/Spites/LevelGeometry/PS_Water"));
	if (WaterTemplate.Succeeded())
	{
		Water = WaterTemplate.Object;
	}

	static ConstructorHelpers::FObjectFinder<UPaperSprite> WoodTemplate(TEXT("/Game/Spites/LevelGeometry/PS_Wood"));
	if (WoodTemplate.Succeeded())
	{
		Wood = WoodTemplate.Object;
	}

#pragma endregion

}

void APieceBase::SetPieceType(EPieceType NewPieceType)
{
	PieceType = NewPieceType;
}
