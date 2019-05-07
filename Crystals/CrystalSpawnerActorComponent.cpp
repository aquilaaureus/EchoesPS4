// Fill out your copyright notice in the Description page of Project Settings.

#include "CrystalSpawnerActorComponent.h"
#include "Actors/Kira/EchoesCharacter.h"
#include <Kismet/GameplayStatics.h>


// Sets default values for this component's properties
UCrystalSpawnerActorComponent::UCrystalSpawnerActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCrystalSpawnerActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCrystalSpawnerActorComponent::TickComponent(float fDeltaTime, ELevelTick eTickType, FActorComponentTickFunction* sThisTickFunction)
{
	Super::TickComponent(fDeltaTime, eTickType, sThisTickFunction);
	
	//Remove dead elements if any
	if ( m_apcTimedDespawningActors.Num() > 0 ) 
	{
		UpdateDespawningActors( fDeltaTime );
	}
}


void UCrystalSpawnerActorComponent::SpawnCrystals ( )
{

	UE_LOG ( LogTemp , Warning , TEXT ( "Crystal spawn called" ) )
	CheckCrystalStatus ( );

	//if crystal object is not setup, return
	if ( !m_pcIceCrystal||( m_bShouldWaitForDestroy && m_apcSpawnedCrystals.Num ( ) > 0 ) ) { return; }

	UE_LOG(LogTemp, Warning , TEXT("Crystal spawn called and should spawn"))

	ACharacter* pcPlayerCharacter = UGameplayStatics::GetPlayerCharacter ( GetWorld ( ) , 0 );

	FTransform sPlayerTransform = pcPlayerCharacter->GetTransform ( );

	for ( int i = 0; i<3; i++ )
	{
		FVector sForwardVector;
		FTransform sSpawnTransform = pcPlayerCharacter->GetTransform ( );
		FVector sSpawnLocation = sSpawnTransform.GetLocation ( );

		switch ( i )
		{
			case 0:
				sForwardVector = pcPlayerCharacter->GetActorForwardVector ( );
				sSpawnLocation -= ( sForwardVector * m_fDistanceFactor );
				break;
			case 1:
				sForwardVector = pcPlayerCharacter->GetActorRightVector ( );
				sSpawnLocation -= ( sForwardVector * m_fDistanceFactor );
				break;
			default:
				sForwardVector = pcPlayerCharacter->GetActorRightVector ( );
				sSpawnLocation += ( sForwardVector * m_fDistanceFactor );
				break;
		}

		if ( m_bIsFromBottom )
		{
			pcPlayerCharacter = UGameplayStatics::GetPlayerCharacter ( GetWorld ( ) , 0 );
			sSpawnLocation.Z = pcPlayerCharacter->GetTransform ( ).GetLocation ( ).Z;

		}
		else
		{
			sSpawnLocation.Z += ( 2.0f * m_fDistanceFactor );
		}
		sSpawnTransform.SetLocation ( sSpawnLocation );

		/*UE_LOG ( LogTemp , Warning , TEXT ( "Player transform : %f , %f , %f " ) , forwardVector.X , forwardVector.Y , forwardVector.Z )
		UE_LOG ( LogTemp , Warning , TEXT ( "transform : %f , %f , %f " ) , spawnTransform.GetLocation ( ).X , spawnTransform.GetLocation ( ).Y , spawnTransform.GetLocation ( ).Z )*/

		AActor * pcSpawnedCrystal = GetWorld ( )->SpawnActor<AActor> ( m_pcIceCrystal , sSpawnTransform );

		m_apcSpawnedCrystals.Add ( pcSpawnedCrystal );
	}

}

void UCrystalSpawnerActorComponent::SpawnCrystalWithTransform( FTransform sSpawnTransform, float fDespawnTimer )
{
	//If element to be spawned is valid
	if ( IsValid(m_pcIceCrystal) ) 
	{
		FActorSpawnParameters sSpawnParams = FActorSpawnParameters();
		sSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* pcCurrentActor = GetWorld()->SpawnActor<AActor>( m_pcIceCrystal, sSpawnTransform, sSpawnParams );
		if ( fDespawnTimer > 0.0f ) 
		{
			m_apcTimedDespawningActors.Add( pcCurrentActor );
			m_afDespawningActorsTimers.Add( fDespawnTimer );
		}
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("CrystalSpawner on Spectre missing IceCrystal reference!"));
	}
}

void UCrystalSpawnerActorComponent::CheckCrystalStatus ( )
{
	// Create an array of crystals that will store those needing to be removed
	TArray<AActor*> apcCrystalsToRemove;

	for ( AActor * pcCrystal:m_apcSpawnedCrystals )
	{
		if ( !pcCrystal->CheckStillInWorld ( ) )
		{
			apcCrystalsToRemove.Add ( pcCrystal );
		}
	}

	for ( AActor * pcCrystal:apcCrystalsToRemove )
	{
		m_apcSpawnedCrystals.Remove ( pcCrystal );
	}
}

void UCrystalSpawnerActorComponent::UpdateDespawningActors( float fDeltaTime )
{
	//Remove and delete 'dead' actors
	for (int iLoop = 0; iLoop < m_apcTimedDespawningActors.Num(); ++iLoop)
	{
		//If the remaining life of the crystal is LTE 0, kill and remove
		if ( ( m_afDespawningActorsTimers[iLoop] - fDeltaTime ) < 0.0f ) 
		{
			if ( IsValid( m_apcTimedDespawningActors[iLoop] ) )
			{
				m_apcTimedDespawningActors[iLoop]->SetActorHiddenInGame( true );
				m_apcTimedDespawningActors[iLoop]->SetActorEnableCollision( false );
				m_apcTimedDespawningActors[iLoop]->SetActorTickEnabled( false );
				m_apcTimedDespawningActors[iLoop]->Destroy();
			}

			//Array resizes here for both arrays.
			m_afDespawningActorsTimers.RemoveAt( iLoop );
			m_apcTimedDespawningActors.RemoveAt( iLoop );
			--iLoop;
		}
		else 
		{
			m_afDespawningActorsTimers[iLoop] -= fDeltaTime;
		}
	}
}

