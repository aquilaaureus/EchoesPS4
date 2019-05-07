// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashGhostComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOES_API UDashGhostComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDashGhostComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Extra delay before the first ghost is spawned (ms)
	UPROPERTY(EditAnywhere, DisplayName = "Initial Delay (ms)")
	float m_fStartDelay;

	//Between spawning Ghosts (ms)
	UPROPERTY(EditAnywhere, DisplayName = "Delay (ms)")
	float m_fSpawnDelay;

	//Number of Ghosts to Spawn
	UPROPERTY(EditAnywhere, DisplayName = "Ghost Number")
	int m_iTotalGhosts;

	int m_iNumOfGhosts;

	float m_fDelayCounter;

	UPROPERTY(EditAnywhere, DisplayName = "Ghost Actor")
	TSubclassOf<AActor> m_pcGhostBP;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void SpawnStart(UActorComponent* pcComponent, bool bReset);
	
};
