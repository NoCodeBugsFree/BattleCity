// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookActor.h"
#include "PickupBase.generated.h"


UENUM(BlueprintType)
enum class EPickupType : uint8
{
	PT_Bomb		UMETA(DisplayName = "Bomb"),
	PT_Clock	UMETA(DisplayName = "Clock"),
	PT_Helmet	UMETA(DisplayName = "Helmet"),
	PT_Life		UMETA(DisplayName = "Life"),
	PT_Shovel	UMETA(DisplayName = "Shovel"),
	PT_Star		UMETA(DisplayName = "Star")
};

/**
* Generic pickup class 
*/

UCLASS()
class BATTLECITY_API APickupBase : public APaperFlipbookActor
{
	GENERATED_BODY()

	/* Root  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneRootComponent;
	
public:

	/** calls when player pick up the pickup   */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAA")
	void OnPickup(APlayerTank* PlayerTank);
	virtual void OnPickup_Implementation(APlayerTank* PlayerTank);

protected:

	APickupBase();
	
	/** Sound to play when pickups */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* PickupSound;

private:

	/** the type of this pickup  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	EPickupType PickupType = EPickupType::PT_Star;
};
