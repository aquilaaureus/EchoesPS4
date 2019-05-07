// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HoundAnimInstance.generated.h"

/**
 * 
 */
UCLASS( transient, Blueprintable, hideCategories = AnimInstance, BlueprintType )
class ECHOES_API UHoundAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// TODO instead of using booleans remake by using enums to change state from one to another
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "Idle Trigger" )
	bool m_bIdleTrigger;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "Attack Trigger" )
	bool m_bAttackTrigger;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "Grab Trigger" )
	bool m_bGrabTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "Death Trigger")
	bool m_bDeathTrigger;

	// Sets the trigger to play idle animation
	void PlayIdleAnimation();

	// Sets trigger to play attack animation
	void PlayAttackAnimation();

	// Sets trigger to play grab animation
	void PlayGrabbingAnimation();

	// Sets the trigger to play the Death Animation
	void PlayDeathAnimation();
};
