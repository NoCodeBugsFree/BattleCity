// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PlayerTank.generated.h"


/** describes all available tank's animation states  */
UENUM(BlueprintType)
enum class ETankAnimState : uint8
{
	Idle_Lvl_00,
	Idle_Lvl_01,
	Idle_Lvl_02,
	Idle_Lvl_03,
	Moving_Lvl_00,
	Moving_Lvl_01,
	Moving_Lvl_02,
	Moving_Lvl_03,
	BeginingPlay
};

/**
 * 
 */
UCLASS()
class BATTLECITY_API APlayerTank : public APaperCharacter
{
	GENERATED_BODY()
	
	// Camera Boom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	// Our in-game camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TankCamera;
	
	/** projectile spawn point  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ProjectileSpawnPoint;

	/** Paper Flipbook Component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* InvulnerableFlipbook;

	/** Movement Audio Component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* MovementAudio;
	
public:
	
	/** calls to add a star to the players player state  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void AddStar();

	/** calls to handle tank behavior on ice
	* BP_IcePiece intended to call this function when player begin overlap it
	*/
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void IceSliding(bool bOnIce);

	/** call to nullify current active projectile reference  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void NullifyProjectileReference(class AProjectile* ProjectileToNullify);

	/** Apply damage to this actor.*/
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** calls to add tank killed to player state  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void AddTankKilled() const;

protected:

	// Sets default values for this pawn's properties
	APlayerTank();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	/** stores Game Mode, Player State, Game State references  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetReferences();

	/**  calls to start spawning player  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartSpawning();

	/** X - movement  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void MoveX(float Value);

	/** Y - movement   */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void MoveY(float Value);

	/**  calls to fire */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Fire();

	/** calls to rotate tank in desired direction  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void RotateTank(float Value);

	/** calls on overlap to handle pickup logic  */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** slow projectile template  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	TSubclassOf<class AProjectile> ProjectileSlowTemplate;

	/** fast projectile template  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	TSubclassOf<class AProjectile> ProjectileFastTemplate;

	/** sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	class USoundBase* FireSound;
	
private:

	/** [tick] simple play tank engine sound when it is moving */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void PlayMoveSound();

	/** [tick] calls to update tank animation */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void UpdateAnimation();

	/** [tick] calls to determine current animation  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void CalcAnimation();

	/** calls to set new tank animation state  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetTankAnimState(ETankAnimState NewState) { TankAnimState = NewState; }

	/** calls to go from "spawning" state to "playing" state  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartPlay();

	/** calls to set the tank invulnerable to enemy attacks for InvulnerableTime period  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetInvulnerable(float InvulnerableTime = 15.f);

	/** calls to revert tank to vulnerable state when Invulnerable timer expires  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void DisableInvulnerable();
	
	// -----------------------------------------------------------------------------------

	/** player initial location to respawn after death at this point  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FVector StartLocation;

	/**  player controller reference */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class APlayerController* PlayerController;

	/** game mode reference  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ABattleCityGameModeBase* BattleCityGameMode;

	/** player state reference  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ABattleCityPlayerState* BattleCityPlayerState;

	/** game state reference  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ABattleCityGameState* BattleCityGameState;

	/** 1st projectile reference
	*   reference to current projectile when it is flying
	*   and will be nullified by it when it destroyed 
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AProjectile* Projectile01;

	/** 2nd projectile reference
	*	used for double fire ability
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AProjectile* Projectile02;

	/** invulnerable timer  */
	UPROPERTY()
	FTimerHandle TimerInvulnerable;

	/** when tank has spawned it will be invulnerable for this short amount of time  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float InitialInvulnerableTime = 2.f;

	//~ ------------- F L I P B O O K S ----------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* BeginPlayFlipbook;

	/** idle  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* IdleStar0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* IdleStar1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* IdleStar2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* IdleStar3;
	
	/** movement  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* MovingStar0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* MovingStar1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* MovingStar2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* MovingStar3;
	
	//~ ------------- F L I P B O O K S ----------------------------------------------------------------------------

	/** current movement vector  */
	FVector MovementVector;

	/** shows whether we is moving on X axis or not  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bMovingX : 1;

	/** shows whether tank is invulnerable or not  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bInvulnerable : 1;
	
	/** current tank animation state   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	ETankAnimState TankAnimState = ETankAnimState::BeginingPlay;
};
