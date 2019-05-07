// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ChestAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ECHOES_API UChestAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Used to control the chest opening
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "Open" )
	bool m_bIsOpen = false;

	// Used to trigger events after the chest has finished opening
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "Opening Complete" )
	bool m_bOpeningComplete = false;
};
