// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectIndexer.h"

// Sets default values
AObjectIndexer::AObjectIndexer( const class FObjectInitializer& ObjectInitializer ) : AActor(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false; //disable tick
}

// Called when the game starts or when spawned
// We set the static caller so that we can get the Indexer from every object
//This way there will be no "major" problems if there are more than one Object Indexers in the level, as only the latest one to enter play will be used.
void AObjectIndexer::BeginPlay()
{
	AActor::BeginPlay();
	staticIndexer = this;
}

// Called every frame - NEVER called. Disabled
void AObjectIndexer::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
}

//We call this to kill an object.
void AObjectIndexer::KillObject( Indexable * objToKill )
{
	int index = m_pcCoord->GetClassIndex( objToKill->GetClassID() );

	if ( index >= m_iNumOfRegisteredClasses )
	{
		ArrayOfPools[index] = new Pool(objToKill->GetClassID());
		++m_iNumOfRegisteredClasses;
	}

	objToKill->KillMe();
	ArrayOfPools[index]->AddObjectToDead( objToKill );
}

//Use this to ressurrect an object at the specified location
//Return true if successful, false otherwise. Most usual reason for failure is that there are no "dead" objects of the class (TypeID) requested.
bool AObjectIndexer::ReviveObjectAt( TypeID classID, FVector pos)
{

	int index = m_pcCoord->GetClassIndex( classID );

	if ( index >= m_iNumOfRegisteredClasses )
	{
		return false;
	}

	if ( index < 0 )
	{
		return false;
		//If you hit this, it means you run out of managed classes (array space).
		//Go and increase the number in the header file
	}

	if ( ArrayOfPools[index]->ReviveObjectAt( pos ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//Use this to ressurrect an object at the specified location
//Return a pointer to the resurrected object if successful, nullptr otherwise. Most usual reason for failure is that there are no "dead" objects of the class (TypeID) requested.
//ALWAYS check return for nullpointer when using this.
Indexable * AObjectIndexer::ReviveObjectAtAndReturn( TypeID classID, FVector pos )
{
	int index = m_pcCoord->GetClassIndex( classID );

	if ( index >= m_iNumOfRegisteredClasses )
	{
		return nullptr;
	}

	if ( index < 0 )
	{
		return nullptr;
	}

	return ArrayOfPools[index]->ReviveObjectAt( pos );
}

//Static getter function for the object indexer
AObjectIndexer * AObjectIndexer::GetObjectIndexer()
{
	return staticIndexer;
}
