// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "EnemyInterface.h"
#include "EnemyTank.generated.h"

/** describes all available tank's animation states  */
UENUM(BlueprintType)
enum class EEnemyTankType : uint8
{
	Base,
	Fast,
	Power,
	Heavy
};

/** describes all available tank's animation states  */
UENUM(BlueprintType)
enum class EEnemyTankAnimState : uint8
{
	Moving,
	BeginingPlay, 
	Frozen
};

/**
 * 
 */
UCLASS()
class BATTLECITY_API AEnemyTank : public APaperCharacter, public IEnemyInterface
{
	GENERATED_BODY()

	/* TODO test need we or not  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;
	
	/** projectile spawn point  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ProjectileSpawnPoint;

	/** invulnerable flipbook component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* InvulnerableFlipbook;
	
public:
	
	//~ Begin Enemy Interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAA")
	void Freeze(bool bFrozen);
	virtual void Freeze_Implementation(bool bNewFrozen);
	//~ End Enemy Interfaces

	/** call to nullify current active projectile reference  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void NullifyProjectileReference(class AProjectile* ProjectileToNullify);

	/** Apply damage to this actor.*/
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** calls to set new tank animation state  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetTankAnimState(EEnemyTankAnimState NewState);

protected:

	AEnemyTank();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/** [tick] calls to animate sprite color only for enemies that have bonus  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void AnimateColor(float DeltaSeconds);

	/** calls to determine and set whether this enemy will have a bonus  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void InitBonusState();

	/** calls to fire   */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Fire();

	/** projectile template class   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSubclassOf<class AProjectile> ProjectileTemplate;

	/** sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	class USoundBase* FireSound;

	/** game mode reference  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ABattleCityGameModeBase* BattleCityGameMode;

private:

	/** calls to rotate tank in random direction  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void RandomRotateTank();

	/** currant tank health   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Health = 1.f;

	/** shows how fast should bonus tank change its color  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float BonusAnimSpeed = 3.f;
	
	/** minimum time (in sec) to next random rotation  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 RotationTimeMin = 2;

	/** maximum time (in sec) to next random rotation  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 RotationTimeMax = 4;

	/** shows whether this tank generate bonus on hit or not  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bHasBonus : 1;

	/** probability to spawn with bonus for this enemy  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 BonusProbability = 10;

	/** delay to next fire  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float FireDelay = 0.6f;

	/** initial delay for visualizing spawning  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float TankSpawningTime = 2.f;

	/** current tank animation state  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	EEnemyTankAnimState TankAnimState = EEnemyTankAnimState::BeginingPlay;

	/** current tank type   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	EEnemyTankType TankType = EEnemyTankType::Base;

	/** projectile reference
	*   reference to current projectile when it is flying
	*   and will be nullified by it when it destroyed
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AProjectile* Projectile;

	//~ ------------- F L I P B O O K S ----------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* BeginPlayFlipbook;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* MovingFlipbook;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* FrozenFlipbook;
	//~ ------------- F L I P B O O K S ----------------------------------------------------------------------------

};
