// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASMeleeEnemy.generated.h"


// Fwd Declare
class UAIHealthWidgetComponent;
class UArrowComponent;
class UBehaviorTree;

//delegate declaration for the melee enemy damage
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FEnemyTakeDamageDelegate , float , fDamageValue );

//delegate declaration for the melee enemy die
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FEnemyDieDelegate, int, iID );

UCLASS(Blueprintable)
class ECHOES_API AASMeleeEnemy : public ACharacter
{
	GENERATED_BODY()
private:
	// Last time the player was spotted
	float m_fLastSeenTime;

	// Last time we attacked something
	float m_fSinceLastAttack;

	// Time-out value to clear the sensed position of the player. Should be higher than Sense interval in the PawnSense component not never miss sense ticks.  
	UPROPERTY( EditDefaultsOnly, Category = "AI" )
	float m_fSenseTimeOut;

	// Resets after sense time-out to avoid unnecessary clearing of target each tick  
	bool m_bSensedTarget;

	//Health
	UPROPERTY( EditDefaultsOnly, Category = "AI" )
	float m_fHealth;

	// Max Health
	UPROPERTY( EditDefaultsOnly, Category = "AI")
	float m_fMaxHealth;

	//An actor comprising of each of the individual meshes of the hound, correctly arranged.
	UPROPERTY(EditDefaultsOnly, Category = "Broken Up Mesh")
	TSubclassOf<AActor> m_pcBrokenUpMesh;

	UPROPERTY( EditAnywhere)
	FVector FHealthWidgetPos;

	// Actor to damage after few seconds into animation
	AActor* m_pcHitActor;

	//  Anim instance to control which animation to play
	class UHoundAnimInstance* m_pcAnimInstance;

	UPROPERTY( VisibleAnywhere, Category = "AI" )
	class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY (Transient)
	UAIHealthWidgetComponent* m_pcAIHealthWidget;

	int m_iMastermindID;

	// The thinking part of the brain, steers our zombie and makes decisions based on the data we feed it from the Blackboard  
	// Assigned at the Character level (instead of Controller) so we may use different zombie behaviors while re-using one controller.  
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* m_pcBehaviorTree;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Triggered by pawn sensing component when a pawn is spotted
	// When using functions as delegates they need to be marked with UFUNCTION(). We assign this function to FSeePawnDelegate
	UFUNCTION()
		void OnSeePlayer(APawn* Pawn);

	// Deal damage to the Actor that was hit by the punch animation  
	UFUNCTION(BlueprintCallable, Category = "Attacking")
		void PerformMeleeStrike(AActor* HitActor);

	UPROPERTY(EditAnywhere, Category = "Attacking")
		float m_fMeleeDamage;

	// Minimum time between melee attacks  
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attacking", DisplayName = "Melee Strike Cooldown")
		float m_fMeleeStrikeCooldown;

public:

	AASMeleeEnemy( FVTableHelper& Helper ) 
		: ACharacter( Helper)
	{
	};

	AASMeleeEnemy( const class FObjectInitializer& ObjectInitializer );

	// Function returns current health
	UFUNCTION(BlueprintCallable, Category = "AI Health")
	float GetCurrentHealth() { return m_fHealth; }

	// Function returns max health
	UFUNCTION(BlueprintCallable, Category = "AI Health")
	float GetMaxHealth() { return m_fMaxHealth; }

	// Reset the Health back to full - Used when the player has died
	UFUNCTION( BlueprintCallable, Category = "AI Health")
	void SetCurrentHealth() { m_fHealth = m_fMaxHealth; }

	// Get the Health Widget Component of the AI Instance 
	UFUNCTION( BlueprintCallable, Category = "AI Health")
	UAIHealthWidgetComponent* GetAIHealthComp() { return m_pcAIHealthWidget; }

	// The actor class that forms the mana pickup to be spawned when the pot is broken
	UPROPERTY( EditAnywhere, Category = "Mana" )
	TSubclassOf<AActor> m_pcManaBP;

	// The actor class that forms the health pickup to be spawned when the pot is broken
	UPROPERTY( EditAnywhere, Category = "Health" )
	TSubclassOf<AActor> m_pcHealthBP;

	//delegate to trigger an event in blueprint when the enemy gets the damage
	UPROPERTY ( BlueprintAssignable , Category = "Enemy Damage" )
	FEnemyTakeDamageDelegate m_dEnemyTakeDamageDelegate;

	//delegate to trigger an event in blueprint when the enemy gets the damage
	UPROPERTY ( BlueprintAssignable , Category = "Enemy Die" )
	FEnemyDieDelegate m_dEnemyDieDelegate;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called by the engine when damage is dealt to this actor
	virtual float TakeDamage( float fDamageAmount, struct FDamageEvent const& rDamageEvent, AController* pEventInstigator, AActor* pDamageCauser ) override;

	UFUNCTION( BlueprintCallable, Category = "Enemy" )
	bool IsAlive();

	// Deals damage to a player called from notify that is triggered at the end of attack animation
	UFUNCTION( BlueprintCallable, Category = "Enemy" )
	void DealDamageToPlayer();

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void KillMe();

	//=========== Variables & Functions for AI Rework Compatability ==============

	//Is this AI currently attacking the enemy
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AI Rework", DisplayName = "Is Attacking" )
	bool m_bIsAttacking;

	//Is this AI currently attached/grabbing player
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AI Rework", DisplayName = "Is Attached To Player" )
	bool m_bIsAttachedToPlayer;

	//How long before this AI can attach to the player again
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AI Rework", DisplayName = "Attachment Delay" )
	float m_fAttachmentDelay;

	//How far away can the Hound attack the player
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AI Rework", DisplayName = "Basic Attack Radius" )
	float m_fBasicAttackRadius;

	//How far away can the Hound grab the player (for latching onto)
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AI Rework", DisplayName = "Grab Range" )
	float m_fGrabRange;

	//Show Attack range with a debug sphere?
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AI Rework", DisplayName = "Show Attack Range" )
	bool m_bShowAttackRange;

	//When the player is further than this from this enemy, the enemy will not react to notifications
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Rework", DisplayName = "Max Notification Range")
	float m_fNotificationReactDistance;

	//Remove this AI from the player
	UFUNCTION( BlueprintCallable, Category = "AI Rework" )
	void DetachFromPlayer();

	//TestAI version of Attack call that uses a custom collision without using collision component.
	//Uses m_fBasicAttackRange and MultiSphereTraceForObjects for proximity check (NOT FOR HIT CHECKING - see HoundAnimNotify.h)
	UFUNCTION( BlueprintCallable, Category = "AI Rework" )
	void BasicAttackPlayer();

	//TestAI for grab check and enabling animation (attach called by anim notify)
	UFUNCTION( BlueprintCallable, Category = "AI Rework" )
	void JumpAtPlayer();
	
	//TestAI replacing the behavior tree task node for attaching to the player
	UFUNCTION( BlueprintCallable, Category = "AI Rework" )
	void AttachToPlayer();

	//Return hound to idle animation
	UFUNCTION( BlueprintCallable, Category = "AI Rework" )
	void ReturnToIdle();

	//============================================================================

	UFUNCTION()
	void NotifyPlayerFound(APawn* Pawn);

	UFUNCTION()
	void RemovedFromPlay(AActor* actor, EEndPlayReason::Type reason);

	inline int GetMastermindID() {
		return m_iMastermindID;
	}

	FORCEINLINE UBehaviorTree* GetTree() const {
		return m_pcBehaviorTree;
	}
};
