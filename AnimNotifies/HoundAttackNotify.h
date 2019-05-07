// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HoundAttackNotify.generated.h"

//fwd decls
class AASMeleeEnemy;

/**
 * 
 */
UCLASS()
class ECHOES_API UHoundAttackNotify : public UAnimNotify
{
	GENERATED_BODY()
	
	
protected:
	virtual void Notify( USkeletalMeshComponent* pcMeshComp, UAnimSequenceBase* pcAnimation ) override;

private:
	//Do a proximity and directional check from the hound to check if we're 'hitting' the player
	bool CheckPlayerIsHit( AASMeleeEnemy* pcHound );
};
