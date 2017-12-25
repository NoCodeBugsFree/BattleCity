// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattleCityPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BATTLECITY_API ABattleCityPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/** calls to play game over animation  */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void PlayGameOverAnimation();
};
