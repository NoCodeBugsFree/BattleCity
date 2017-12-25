// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLECITY_API IEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//~ Begin Enemy Interface

	/** calls to freeze all enimies for a short period of time  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAA")
	void Freeze(bool bFrozen);
	virtual void Freeze_Implementation(bool bNewFrozen);
	
	//~ End Enemy Interfaces
	
	
};
