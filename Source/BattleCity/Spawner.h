// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS()
class BATTLECITY_API ASpawner : public AActor
{
	GENERATED_BODY()

	// Our in-game camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* LevelCamera;

	/* scene root component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;
	
	/** black  background sprite */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* BlackBackGroundSprite;

	/** gray background sprite */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* GrayBackGroundSprite;
	
	/* box that covers the whole map to spawn pickups  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PickupSpawningBox;

	/* 1st enemy spawn point  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* EnemySpawnPoint01;

	/* 2nd enemy spawn point  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* EnemySpawnPoint02;

	/* 3rd enemy spawn point  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* EnemySpawningPoint3;

	/* 1st blocking volume */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BlockingBox01;

	/* 2ns blocking volume */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BlockingBox02;

	/* 3rd blocking volume */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BlockingBox03;

	/* 4th blocking volume */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BlockingBox04;

public:

	/** calls by internal timer to spawn required amount of enemies */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SpawnEnemy();

	/** call by GameMode to spawn pickup when player hits enemy that has bonus  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SpawnPickup();
	
	/** call by GameMode to spawn pickup when player hits enemy that has bonus  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void RemoveEnemy(AActor*  TankToRemove);

	/** returns how many enemies we need to spawn   */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	int32 GetEnemiesAmount() const { return EnemiesToSpawn.Num(); }

	/** next level name to travel after current level will be finished  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FString NextLevelName = FString("Map02");

	/** all enemy type templates to spawn during current level  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class AEnemyTank>> EnemiesToSpawn;

	/** current list of enemies of battle field  */
	UPROPERTY()
	TArray<class AActor*> EnemiesOnField;

protected:

	// Sets default values for this actor's properties
	ASpawner();

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
private:

	// -------- Enemy Tank templates -------------------------------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AEnemyTank> BaseTankTemplate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AEnemyTank> FastTankTemplate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AEnemyTank> PowerTankTemplate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AEnemyTank> HeavyTankTemplate;

	// --------  Enemy Tank templates -------------------------------------------------------
	
	/** enemies amount to spawn during this level  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true", EditCondition = "bRandomEnemies", ClampMin = 1, ClampMax = 100, UIMin = "1", UIMax = "100"))
	int32 EnemiesAmount = 10;

	/** shows whether spawner needs to auto generate enemies types or this list   */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bRandomEnemies : 1;

	/**  shows whether we need to use player camera or level camera */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bUseSpawnerCamera : 1;

	/** game state reference */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ABattleCityGameState* BattleCityGameState;
	
	/** shows how many enemies can be spawned on the battle field  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 MaxEnemiesOnField = 4;

	/** current active enemies on the battle field   */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 CurrentEnemiesOnField = 0;

	/** delay to spawn next enemy  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float EnemySpawnDelay = 1.f;

	/** list of all available pickups  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class APickupBase>> PickupTypes;

public:
	
	/** returns level camera component **/
	FORCEINLINE class UCameraComponent* GetLevelCamera() const { return LevelCamera; }

};
