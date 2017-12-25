// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "PieceBase.generated.h"

/** the state of base piece
*   according to this state in BP construction script intended to create 
*   different pieces for all level geometry actors
*/
UENUM(BlueprintType)
enum class EPieceType : uint8
{
	Brick,
	Steel, 
	Ice, 
	Wood,
	Water
};

UCLASS()
class BATTLECITY_API APieceBase : public APaperSpriteActor
{
	GENERATED_BODY()

	// ---------- Sprite templates --------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSprite* Brick;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSprite* Steel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSprite* Ice;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSprite* Water;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSprite* Wood;
	// ---------- Sprite templates --------------------------------------------------------------

public:	

	/** calls to set new state of piece  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetPieceType(EPieceType NewPieceType);
	
protected:

	// Sets default values for this actor's properties
	APieceBase();

	/** represents the type of piece  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	EPieceType PieceType = EPieceType::Brick;

public:

	/** returns paper sprite  */
	class UPaperSpriteComponent* GetPaperSpriteComponent() const { return GetRenderComponent(); }
};
