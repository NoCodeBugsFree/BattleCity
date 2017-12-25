// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleCityGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BATTLECITY_API ABattleCityGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	/** calls when player pickup the bomb pickup  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void KillAllEnemies(class APlayerTank* Killer);

	/**  calls when player pickup the clock pickup  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void FreezeAllEnemies(bool bNewFreeze = false);

	/**  calls when player pickup the shovel pickup  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ProtectBase();

	/** calls to spawn a bonus pick up  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SpawnBonus();

	/** calls when base is destroyed or player lost all his lives  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void GameOver();

	/** calls to set player base reference in game mode  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SetPlayerBaseRef(class APlayerBase* MilitaryBaseToSet);

	/**  store spawner reference and check that it is alone at map */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SetSpanerRefAndInitGameState();

	/** calls to check whether all enemies are killed or not  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void CheckForNextLevel();

protected:

	ABattleCityGameModeBase();

	virtual void BeginPlay() override;
	
private:

	/** game state reference  */
	UPROPERTY()
	class ABattleCityGameState* BattleCityGameState;

	/** player base reference  */
	UPROPERTY()
	class APlayerBase* PlayerBase;

	/** spawner reference  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ASpawner* Spawner;

	/** the name of menu map to load it after game over  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FString MenuMapName = FString("Menu");
	
	/** shows how many time (sec) we delay before start a next level  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DelayForNextLevel = 3.f;
	
	/** time to freeze enemies when player pick up a clock pickup  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 FreezeTime = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundBase* GameOverSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundBase* GameStartSound;

};
