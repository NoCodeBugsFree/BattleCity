// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplodeFX.generated.h"

UCLASS()
class BATTLECITY_API AExplodeFX : public AActor
{
	GENERATED_BODY()
	
	/** explode  flipbook */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* ExplodeFlipbook;
	
protected:

	// Sets default values for this actor's properties
	AExplodeFX();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Sound to play when explodes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	class USoundBase* ExplodeSound;

	/** calls to simple destroy self after explode flip book finished playing  */
	UFUNCTION()
	void DestroySelf();
	
};
