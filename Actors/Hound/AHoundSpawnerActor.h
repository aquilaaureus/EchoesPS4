// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASMeleeEnemy.h"
#include "GameFramework/Actor.h"
#include "AHoundSpawnerActor.generated.h"

UCLASS()
class ECHOES_API AAHoundSpawnerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAHoundSpawnerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void RemovedFromPlay(AActor* pcActor, EEndPlayReason::Type EReason);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
