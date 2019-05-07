// Fill out your copyright notice in the Description page of Project Settings.

#include "GhostSpawnComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UGhostSpawnComponent::UGhostSpawnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UGhostSpawnComponent::SpawnGhost( FVector vLocation, FRotator vRotation )
{
	UGameplayStatics::BeginSpawningActorFromBlueprint( GetWorld(), m_pcGhost, FTransform( vRotation, vLocation ), true );
}

// Called when the game starts
void UGhostSpawnComponent::BeginPlay()
{
	Super::BeginPlay();
}


