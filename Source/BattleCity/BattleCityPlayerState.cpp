// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleCityPlayerState.h"
#include "Net/UnrealNetwork.h"

/** Returns properties that are replicated for the lifetime of the actor channel */
void ABattleCityPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABattleCityPlayerState, Lives);
	DOREPLIFETIME(ABattleCityPlayerState, TanksKilled);
	DOREPLIFETIME(ABattleCityPlayerState, Stars);
}

void ABattleCityPlayerState::AddStar()
{
	Stars = FMath::Clamp(Stars++, 0, 3);
}
