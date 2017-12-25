// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleCityGameState.generated.h"

/**
 * 
 */
UCLASS()
class BATTLECITY_API ABattleCityGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	
	/** shows how many enemies we need to kill to finish current level  */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 EnemiesRemaining = 0;

	/** the number of current level   */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 CurrentLevel = 1;

};
