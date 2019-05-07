// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "Indexable.h"
#include "ClassType.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectIndexer.generated.h"

#define MAXREGISTEREDCLASSES 25

UCLASS()
class ECHOES_API AObjectIndexer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObjectIndexer( const class FObjectInitializer& ObjectInitializer );

	//the coordinator is used to retrieve the index of the pool we want from the pool array
	class Coordinator
	{
	private:
		char** registeredclasses = new TypeID[MAXREGISTEREDCLASSES];
		int numOfRegisteredClasses = 0;

	public:
		Coordinator()
		{
			registeredclasses = new TypeID[MAXREGISTEREDCLASSES];
			numOfRegisteredClasses = 0;
		}

		~Coordinator()
		{
			delete[] registeredclasses;
		}

		int GetClassIndex( TypeID classID )
		{
			for ( int i = 0; i < numOfRegisteredClasses; ++i )
			{
				if ( registeredclasses[i] == classID )
				{
					return i;
				}
			}

			if ( numOfRegisteredClasses < MAXREGISTEREDCLASSES )
			{
				registeredclasses[numOfRegisteredClasses++] = classID;
				return numOfRegisteredClasses - 1;
			}
			else
			{
				return -1;
			}
		}
	};

	//Each object should have a specific pool. The infrastructure exists for checks of TypeIDs, but they are not needed now since Pools are generated proceduarally as needed.
	class Pool
	{
		char* HandlingClassID;
		std::vector<Indexable*>* deadVector = new std::vector<Indexable*>();

	public:
		Pool( char* classid )
		{
			HandlingClassID = classid;
		}

		inline char* GetHandlingClassID()
		{
			return HandlingClassID;
		}

		Indexable* ReviveObjectAt(FVector pos)
		{
			if ( deadVector->empty() )
			{
				return nullptr;
			}
			else
			{
				Indexable* objToReturn = deadVector->back();
				deadVector->pop_back();
				objToReturn->RessurrectMeAt( pos );
				return objToReturn;
			}
		}

		void AddObjectToDead( Indexable* object )
		{
			deadVector->push_back( object );
		}
	};
private:

	Coordinator* m_pcCoord = new Coordinator();
	Pool** ArrayOfPools = new Pool*[MAXREGISTEREDCLASSES];
	int m_iNumOfRegisteredClasses = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//call this to kill the object
	void KillObject( Indexable* objToKill );
	
	//Use this to ressurrect an object at the specified location
	//Return true if successful, false otherwise. Most usual reason for failure is that there are no "dead" objects of the class (TypeID) requested.
	bool ReviveObjectAt( TypeID classID, FVector pos );

	//Use this to ressurrect an object at the specified location
	//Return a pointer to the resurrected object if successful, nullptr otherwise. Most usual reason for failure is that there are no "dead" objects of the class (TypeID) requested.
	//ALWAYS check return for nullpointer when using this.
	Indexable* ReviveObjectAtAndReturn( TypeID classID, FVector pos );

	static AObjectIndexer* GetObjectIndexer();
};

//static pointer to active object indexer
static AObjectIndexer* staticIndexer;

#undef MAXREGISTEREDCLASSES
