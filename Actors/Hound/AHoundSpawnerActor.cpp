// Fill out your copyright notice in the Description page of Project Settings.

#include "AHoundSpawnerActor.h"
#include "../Mastermind/AMastermind.h"

// Sets default values
AAHoundSpawnerActor::AAHoundSpawnerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AAHoundSpawnerActor::BeginPlay()
{
	Super::BeginPlay();
	OnEndPlay.AddDynamic(this, &AAHoundSpawnerActor::RemovedFromPlay);

	AMastermind::GetMasterMind(this)->SpawnHoundAtLocation(GetActorLocation());
}

void AAHoundSpawnerActor::RemovedFromPlay(AActor * pcActor, EEndPlayReason::Type EReason)
{


}

// Called every frame
void AAHoundSpawnerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

