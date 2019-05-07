// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "StopChestOpeningNotify.generated.h"

/**
 * 
 */
UCLASS()
class ECHOES_API UStopChestOpeningNotify : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual void Notify ( USkeletalMeshComponent* pcMeshComp , UAnimSequenceBase* pcAnimation ) override;
	
};
