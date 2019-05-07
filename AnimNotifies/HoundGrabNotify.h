// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HoundGrabNotify.generated.h"

//fwd decls
class AASMeleeEnemy;

/**
 * 
 */
UCLASS()
class ECHOES_API UHoundGrabNotify : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual void Notify( USkeletalMeshComponent* pcMeshComp, UAnimSequenceBase* pcAnimation ) override;
	
private:
	//Check the actor location and facing against the player location and facing
	//If the player is close, facing away from the hound & back is available, return true.
	bool CheckPlayerIsGrabbed( AASMeleeEnemy* pcHound );
};
