// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleCityGameModeBase.h"
#include "EngineUtils.h"
#include "EnemyInterface.h"
#include "EnemyTank.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerTank.h"
#include "ExplodeFX.h"
#include "PlayerTank.h"
#include "PlayerBase.h"
#include "BattleCityGameState.h"
#include "BattleCityPlayerController.h"
#include "Spawner.h"

ABattleCityGameModeBase::ABattleCityGameModeBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseSeamlessTravel = true;
}

void ABattleCityGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	/**  store spawner reference and check that it is alone at map */
	SetSpanerRefAndInitGameState();

	/** play game start sound  */
	if (GameStartSound)
	{
		UGameplayStatics::PlaySound2D(this, GameStartSound);
	}
}

void ABattleCityGameModeBase::SetSpanerRefAndInitGameState()
{
	
		/** SPAWNER  */
		TArray<AActor*>Spawners;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawner::StaticClass(), Spawners);

		/**  intended to have one spawner at map */
		if (Spawners.Num() == 1)
		{
			if (Spawners.IsValidIndex(0) && Spawners[0])
			{
				if (ASpawner* TestSpawner = Cast<ASpawner>(Spawners[0]))
				{
					Spawner = TestSpawner;
					
					/** init enemies remaining  */
					if(ABattleCityGameState* TestBattleCityGameState = Cast<ABattleCityGameState>(GetWorld()->GetGameState()))
					{
						BattleCityGameState = TestBattleCityGameState;
						BattleCityGameState->EnemiesRemaining = Spawner->GetEnemiesAmount();
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("One spawner per map !"));
		}
	
}

void ABattleCityGameModeBase::CheckForNextLevel()
{
	
		if (BattleCityGameState && Spawner)
		{
			/** if we have no enemies to spawn and we have haven't enemies on battle field  */
			if (Spawner->EnemiesToSpawn.Num() == 0 && Spawner->EnemiesOnField.Num() == 0)
			{
				/** level up  */
				BattleCityGameState->CurrentLevel++;

				/** open next level by timer  */
				FTimerDelegate NextLevelStartDelegate;
				NextLevelStartDelegate.BindLambda([&] {
					UGameplayStatics::OpenLevel(this, *Spawner->NextLevelName);
				});

				FTimerHandle NextLevelTimer;
				GetWorldTimerManager().SetTimer(NextLevelTimer, NextLevelStartDelegate, DelayForNextLevel, false);
			}
		}
	
}

void ABattleCityGameModeBase::KillAllEnemies(APlayerTank* Killer)
{
	
		if (Killer)
		{
			TArray<AActor*> Enemies;
			UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UEnemyInterface::StaticClass(), Enemies);
			for (AActor* Target : Enemies)
			{
				if (Target)
				{
					/** 101.f - for things like heavy tank  */
					Target->TakeDamage(101.f, FDamageEvent(), Killer->GetController(), nullptr);
				}
			}

			/** check for next level  */
			CheckForNextLevel();
		}
	
}

void ABattleCityGameModeBase::FreezeAllEnemies(bool bNewFreeze)
{
	
		TArray<AActor*> Enemies;
		UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UEnemyInterface::StaticClass(), Enemies);
		for (const auto& Target : Enemies)
		{
			if (IEnemyInterface* Enemy = Cast<IEnemyInterface>(Target))
			{
				Enemy->Execute_Freeze(Target, bNewFreeze);
			}
		}

		/** Unfreeze enemies when freeze time ends   */
		FTimerDelegate TimerDelegate;
		FTimerHandle UnfrezeeTimer;
		bool bNewFreezeState = false;
		TimerDelegate.BindUFunction(this, FName("FreezeAllEnemies"), bNewFreezeState);
		GetWorldTimerManager().SetTimer(UnfrezeeTimer, TimerDelegate, FreezeTime, false);
	
}

void ABattleCityGameModeBase::ProtectBase()
{
	if (PlayerBase)
	{
		PlayerBase->SetInvulnerable();
	}
}

void ABattleCityGameModeBase::SpawnBonus()
{
	if (Spawner)
	{
		Spawner->SpawnPickup();
	}
}

void ABattleCityGameModeBase::GameOver()
{
	// for each player controller
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; It++)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(*It))
		{
			/** play Game over HUD animation  */
			if(ABattleCityPlayerController* PC = Cast<ABattleCityPlayerController>(PlayerController))
			{
				PC->PlayGameOverAnimation();
			}
				
			/** disable player input  */
			if(APlayerTank* PlayerTank = Cast<APlayerTank>(PlayerController->GetPawn()))
			{
				PlayerTank->DisableInput(PlayerController);
			}
		}
	}

	// play game over sound 
	if (GameOverSound )
	{
		UGameplayStatics::PlaySound2D(this, GameOverSound);
	}

	// set game over widget
	/** open menu by timer  */
	FTimerDelegate MenutDelegate;
	MenutDelegate.BindLambda([&] {
		UGameplayStatics::OpenLevel(this, *MenuMapName);
	});

	FTimerHandle MenuTimer;
	GetWorldTimerManager().SetTimer(MenuTimer, MenutDelegate, 5.5f, false);
	
}

void ABattleCityGameModeBase::SetPlayerBaseRef(class APlayerBase* PlayerBaseToSet)
{
	if (PlayerBaseToSet)
	{
		PlayerBase = PlayerBaseToSet;
	}
}

