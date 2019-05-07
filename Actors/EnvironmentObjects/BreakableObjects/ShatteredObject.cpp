// Fill out your copyright notice in the Description page of Project Settings.

#include "ShatteredObject.h"

#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime//Engine/Classes/GameFramework/Pawn.h"

// Sets default values
AShatteredObject::AShatteredObject() :
	m_apcMeshList		( nullptr )
	, m_bPhysicsActive	( true )
	, m_fTimeLimit		( 3.0f )
	, m_ftimePassed		( 0.0f )
	, m_fZSpeed			( 1.0f )
	, m_fForceMult		( 800.0f )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//initialize the mesh list
	m_apcMeshList = new TArray<UStaticMeshComponent*>();
}

// Called when the game starts or when spawned
void AShatteredObject::BeginPlay()
{
	AActor::BeginPlay();
	
	m_bPhysicsActive = true;

	m_ftimePassed = 0.0f;
}

void AShatteredObject::PostInitializeComponents()
{
	AActor::PostInitializeComponents();

	TArray<UActorComponent*> pcMeshlist = GetComponentsByClass(UStaticMeshComponent::StaticClass());

	for (UActorComponent* pcComponent : pcMeshlist)
	{
		UStaticMeshComponent* pcSubmesh = Cast<UStaticMeshComponent>(pcComponent);
		if (pcSubmesh)
		{
			m_apcMeshList->Add(pcSubmesh); //no need to check since we only returned Static Mesh Components from the above function
			pcSubmesh->SetSimulatePhysics(true);
			pcSubmesh->CanCharacterStepUpOn = ECB_No;
			pcSubmesh->SetCollisionResponseToAllChannels(ECR_Block);
			pcSubmesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
			pcSubmesh->OnComponentBeginOverlap.AddDynamic(this, &AShatteredObject::ComponectOverlapBegin);
			
		}
	}
}

// Called every frame
void AShatteredObject::Tick(float fDeltaTime)
{
	AActor::Tick(fDeltaTime);

	m_ftimePassed += fDeltaTime;

	if (m_bPhysicsActive && m_ftimePassed > m_fTimeLimit)
	{
		for ( int i=0;i<m_apcMeshList->Num();++i )
		{
			UStaticMeshComponent* pcSubMesh = m_apcMeshList->GetData()[i];
			if (pcSubMesh)
			{
				//pcSubMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
				//pcSubMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			}
		}

		m_bPhysicsActive = false;
		m_ftimePassed = 0.0f;
	}
	else if (m_ftimePassed >= m_fTimeLimit) //reached only after collisions deactivated
	{
		Destroy();
	}
	else if (m_fZSpeed<1.0f && !m_bPhysicsActive) //reached only after collisions deactivated and before destruction
	{
		for (int i = 0; i < m_apcMeshList->Num(); ++i)
		{
			UStaticMeshComponent* pcSubMesh = m_apcMeshList->GetData()[i];
			if (pcSubMesh)
			{
				//pcSubMesh->AddForce(FVector(0.0f, 0.0f, 980 - m_fZSpeed), NAME_None, true);
				pcSubMesh->SetRelativeScale3D(pcSubMesh->RelativeScale3D*(1.0f - m_fZSpeed*fDeltaTime));
			}
		}
	}
}

void AShatteredObject::ComponectOverlapBegin(UPrimitiveComponent* pcOverlappedComponent, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex, bool bFromSweep, const FHitResult & rsSweepResult)
{
	APawn* pcPawn = Cast<APawn>(pcOtherActor);
	if (pcPawn && m_bPhysicsActive)
	{
		FVector vForce = pcOverlappedComponent->GetComponentLocation() - pcPawn->GetActorLocation();
		vForce.Z = 10.0f;	//always move upwards
		vForce *= m_fForceMult;	//Force Multiplier
		pcOverlappedComponent->AddForce(vForce, NAME_None, true);
	}
}

