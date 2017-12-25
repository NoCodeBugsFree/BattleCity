// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "Projectile.generated.h"

UCLASS()
class BATTLECITY_API AProjectile : public APaperSpriteActor
{
	GENERATED_BODY()
	
	/* scene root component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ROOT;
	
	/* projectile movement component   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
public:

	/** calls to set this projectile to piercing when player obtains 3 stars  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetPiercing(bool bNewPiercing) { bPiercing = bNewPiercing; }

	/** calls to check whether this projectile is piercing or not  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool IsPiercing() const { return bPiercing; }

protected:

	// Sets default values for this actor's properties
	AProjectile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** used for nullify this projectile reference in Owner (player or enemy tank) */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** calls when this projectile overlaps something  */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:

	/** shows whether this projectile is piercing or not  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bPiercing : 1;

	/** damage to cause by this projectile  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DamageToCause = 1.01f;
	
};
