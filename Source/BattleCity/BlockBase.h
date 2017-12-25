// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PieceBase.h"
#include "BlockBase.generated.h"

UCLASS()
class BATTLECITY_API ABlockBase : public AActor
{
	GENERATED_BODY()
	
	/* Root  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ROOT;
	
	// @API A component that spawns an Actor when registered, and destroys it when unregistered.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* Piece01;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* Piece02;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* Piece03;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* Piece04;
	
protected:

	// Sets default values for this actor's properties
	ABlockBase();

private:

	/** piece type for this block  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	EPieceType PieceType = EPieceType::Brick;
	
	
};
