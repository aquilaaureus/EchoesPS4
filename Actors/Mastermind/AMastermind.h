// Fill out your copyright notice in the Description page of Project Settings.

/*
* This class will act as a connector that holds all active hounds and connects them with each other. 
* It will be a static included via code in the levels.
* 
* It will be used to regulate their group behavior (Pack AI)
* 
* It may also be used to control their spawn locations (stream correct behavior) if the health bar issue cannot be fixed
* 
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AMastermind.generated.h"

//Forward Declare
class APawn;
class AASMeleeEnemy;
class ASpecter;

//Player Found Notification Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerFoundDelegate, APawn*, pcFoundPlayer);

struct SDynamicChainList 
{
	SDynamicChainList* m_psNext;
	int m_iID;
	FVector* m_vLocation;
};

UCLASS(Blueprintable)
class ECHOES_API AMastermind : public AActor
{
	GENERATED_BODY()
private:
	int				m_icurrentID;

	ASpecter*		m_pcBoss; //Only one of these

	SDynamicChainList* m_psMasterList;

	SDynamicChainList* m_psAliveHounds;
	SDynamicChainList* m_psDeadHounds;

	void GenerateListItems();

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AMastermind> m_pbItself;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AASMeleeEnemy> m_pbHoundBP;



protected:
	//called before BeginPlay is called for actors, so used to set up things.
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FPlayerFoundDelegate	m_dNotifyPlayerFoundDelegate;

	SDynamicChainList* GetListItem();

public:

	AMastermind( FVTableHelper& Helper ) 
		: AActor( Helper),
		m_pcBoss(nullptr)
	{
	};

	virtual ~AMastermind();

	AMastermind( const class FObjectInitializer& ObjectInitializer );

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//called to notify all currently active hounds that the player is spotted
	void NotifyAliveHounds(APawn* player);

	//called every time an enemy is added in the game
	int EnemySpawned(AASMeleeEnemy* enemy);
	int EnemySpawned(ASpecter* pcBoss);

	//called to inform the mastermind that an enemy has been removed from the level
	void EnemyDied(AASMeleeEnemy* enemy);

	//Called when the enemy broadcasts that they have been killed
	UFUNCTION()
	void EnemyKilled(int iID);

	void SpawnHoundAtLocation(FVector vLocation);

	//static to get the mastermind
	//To call, you either pass "this" from the UObject you are calling, or otherwise the UWorld object (which is a valid reference object)
	//This is needed so that we can create the mastermind if one has not been created.
	static AMastermind* GetMasterMind(UObject* pcWorldRefObject);

};

static AMastermind* spcAMasterMind;