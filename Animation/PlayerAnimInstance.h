// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ECHOES_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	// TODO instead of using booleans remake by using enums to change state from one to another
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "Attack Trigger" )
	int m_bAttackTrigger;

	// TODO instead of using booleans remake by using enums to change state from one to another
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "Attack Finished" )
	bool m_bAttackOneFinished;

	// TODO instead of using booleans remake by using enums to change state from one to another
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "CanDash" )
	bool m_bCanDash;

	// TODO instead of using booleans remake by using enums to change state from one to another
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "CanBlock" )
	bool m_bCanBlock;

	// TODO instead of using booleans remake by using enums to change state from one to another
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "CanShoot" )
	bool m_bCanShoot;
	
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Custom" , DisplayName = "IsDoubleJumping" )
	bool m_bIsDoubleJumping;

	// Used to control opening of a chest
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "OpeningChest")
	bool m_bIsOpeningChest;

	// Used to control death Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom", DisplayName = "IsDead")
	bool m_bIsDead;

};
