// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShatteredObject.generated.h"

class UStaticMeshComponent;

UCLASS()
class ECHOES_API AShatteredObject : public AActor
{
	GENERATED_BODY()
	
protected:
	TArray<UStaticMeshComponent*>* m_apcMeshList;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	float m_ftimePassed;

	bool m_bPhysicsActive;

	//For Collision Deactivation
	UPROPERTY(EditDefaultsOnly, DisplayName = "Time Limit.")
	float m_fTimeLimit;

	UPROPERTY(EditDefaultsOnly, DisplayName = "Shrinking Speed. FPS Expensive.")
	float m_fZSpeed;

	//Force multiplier when a pawn hits one of the objects.
	UPROPERTY(EditDefaultsOnly, DisplayName = "Force Multiplier")
	float m_fForceMult;

public:
	// Sets default values for this actor's properties
	AShatteredObject();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ComponectOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
};
