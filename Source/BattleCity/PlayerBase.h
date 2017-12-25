// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerBase.generated.h"

UCLASS()
class BATTLECITY_API APlayerBase : public AActor
{
	GENERATED_BODY()

	/* scene root component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;

	/**  sprite that represents eagle - base center */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* BaseSprite;

	/** base dead sprite template   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSprite* BaseDead;

	/** steel sprite template   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSprite* SteelSprite;

	/** steel sprite template   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSprite* BrickSprite;

public:	

	/** calls to set player base invulnerable for a short period when player pick up Shovel pickup  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SetInvulnerable();

protected:

	// Sets default values for this actor's properties
	APlayerBase();

	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** calls when center of base (the eagle) overlaps by projectile */
	UFUNCTION()
	void OnBaseCenterOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** the amount of time to be invulnerable when player pick up Shovel pickup   */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float InvulnerableTime = 15.f;

	/** all bricks for this actor will be added as child actor components
	*   we need to gather them into the array to change their state according
	*   the state of this actor (steel it or brick)
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<class APieceBase*> AllBricks;
	
};
