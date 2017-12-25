// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BattleCityPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BATTLECITY_API ABattleCityPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	/** shows how many times player can die to lose the game */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 Lives = 3;

	/** shows how many tanks was killed by player */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 TanksKilled = 0;

	/** shows how many stars have the player */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 Stars = 0;

	/** calls to add a star to the player  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void AddStar();
};
