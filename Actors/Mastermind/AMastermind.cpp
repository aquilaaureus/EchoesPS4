// Fill out your copyright notice in the Description page of Project Settings.

#include "AMastermind.h"
#include "../Hound/ASMeleeEnemy.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "../Spectre/Specter.h"

AMastermind::~AMastermind()
{
	spcAMasterMind = nullptr;
}

AMastermind::AMastermind(const FObjectInitializer & rObjectInitializer)
	: AActor(rObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMastermind::GenerateListItems()
{
	int iCount = 20;
	
	SDynamicChainList* temp = new SDynamicChainList();
	temp->m_psNext = nullptr;
	temp->m_iID = -1;
	temp->m_vLocation = new FVector();
	m_psMasterList = temp;

	while (iCount > 0)
	{
		temp = new SDynamicChainList();
		temp->m_psNext = m_psMasterList;
		temp->m_iID = -1;
		temp->m_vLocation = new FVector();
		m_psMasterList = temp;
		--iCount;
	}
}

void AMastermind::PostInitializeComponents()
{
	AActor::PostInitializeComponents();
	m_icurrentID = 0;
	m_pcBoss = nullptr;
	m_psAliveHounds = nullptr;
	m_psDeadHounds = nullptr;
	spcAMasterMind = this;
	GenerateListItems();
}

//Called when the game starts or when spawned
void AMastermind::BeginPlay()
{
	AActor::BeginPlay();

}

SDynamicChainList * AMastermind::GetListItem()
{
	if ( !(m_psMasterList->m_psNext) ) {
		GenerateListItems();
	}

	SDynamicChainList* psItemToReturn = m_psMasterList;
	m_psMasterList = m_psMasterList->m_psNext;
	psItemToReturn->m_psNext = nullptr;
	return psItemToReturn;
}

//Called on every loop.
void AMastermind::Tick(float fDeltaTime)
{
	AActor::Tick(fDeltaTime);

}

//this is called when a hound spots the enemy
void AMastermind::NotifyAliveHounds(APawn* pcPlayer)
{
	m_dNotifyPlayerFoundDelegate.Broadcast(pcPlayer);
}

int AMastermind::EnemySpawned(AASMeleeEnemy* enemy)
{
	
	m_dNotifyPlayerFoundDelegate.AddDynamic(enemy, &AASMeleeEnemy::NotifyPlayerFound);
	enemy->m_dEnemyDieDelegate.AddDynamic(this, &AMastermind::EnemyKilled);
	return m_icurrentID;

}

int AMastermind::EnemySpawned(ASpecter * pcBoss)
{
	m_pcBoss = pcBoss;
	return 0;
}

void AMastermind::EnemyDied(AASMeleeEnemy* enemy)
{
	m_dNotifyPlayerFoundDelegate.RemoveDynamic(enemy, &AASMeleeEnemy::NotifyPlayerFound);

	SDynamicChainList* curr = m_psAliveHounds;

	if(!curr) 
	{
		return;
	}

	if (curr->m_iID == enemy->GetMastermindID()) {
		m_psAliveHounds = curr->m_psNext;

		curr->m_psNext = m_psMasterList;
		m_psMasterList = curr;
		return;
	}

	while (curr->m_psNext)
	{
		if (curr->m_psNext->m_iID == enemy->GetMastermindID())
		{
			SDynamicChainList* temp = curr->m_psNext;
			curr->m_psNext = temp->m_psNext;
			temp->m_psNext = m_psMasterList;
			m_psMasterList = temp;
			return;
		}
		curr = curr->m_psNext;
	}
}

void AMastermind::EnemyKilled(int iID)
{
	SDynamicChainList* curr = m_psAliveHounds;

	if (curr)
	{
		if (curr->m_iID == iID) {
			m_psAliveHounds = curr->m_psNext;

			curr->m_psNext = m_psDeadHounds;
			m_psDeadHounds = curr;
			return;
		}

		while (curr->m_psNext)
		{
			if (curr->m_psNext->m_iID == iID)
			{
				SDynamicChainList* temp = curr->m_psNext;
				curr->m_psNext = temp->m_psNext;
				temp->m_psNext = m_psDeadHounds;
				m_psDeadHounds = temp;
				return;
			}
			curr = curr->m_psNext;
		}
	}
}

void AMastermind::SpawnHoundAtLocation(FVector vLocation)
{

	SDynamicChainList* curr = m_psDeadHounds;
	
	while (curr)
	{
		if (FVector::DistSquared(vLocation, *curr->m_vLocation) < 10) {
			UE_LOG(LogTemp, Warning, TEXT("Enemy Skipped due to previous killing."));
			return;
		}

		curr = curr->m_psNext;
	}

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (m_pbHoundBP)
	{
		curr = GetListItem();
		curr->m_iID = ++m_icurrentID;
		curr->m_vLocation->X = vLocation.X;
		curr->m_vLocation->Y = vLocation.Y;
		curr->m_vLocation->Z = vLocation.Z;
		curr->m_psNext = m_psAliveHounds;
		GetWorld()->SpawnActor<AASMeleeEnemy>(m_pbHoundBP, vLocation, FRotator::ZeroRotator, params);
		m_psAliveHounds = curr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BP class Spawn Failed!"));
	}
}

AMastermind * AMastermind::GetMasterMind(UObject* pcWorldRefObject)
{
	if (spcAMasterMind)
	{
		return spcAMasterMind;
	}

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	spcAMasterMind = pcWorldRefObject->GetWorld()->SpawnActor<AMastermind>(params);

	UE_LOG(LogTemp, Warning, TEXT("Mastermind spawned."));

	return spcAMasterMind;
}
