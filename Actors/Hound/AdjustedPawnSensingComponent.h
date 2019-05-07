// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/PawnSensingComponent.h"
#include "AdjustedPawnSensingComponent.generated.h"

/**
 * 
 */
UCLASS()
class ECHOES_API UAdjustedPawnSensingComponent : public UPawnSensingComponent
{
	GENERATED_BODY()

		virtual void SensePawn( APawn& Pawn ) override;
	
};
