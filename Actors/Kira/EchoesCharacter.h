 // Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EchoesCharacter.generated.h"

//fwd decls
class AMenuWidgetPlayerController;
class ANPC;
class UCameraComponent;
class UHoundGrappleReciever;
class UInputComponent;
class UMaterialInterface;
class UParticleSystem;
class UPlayerCameraShake;
class UPlayerProjectileEmitter;
class UPlayerStatsComponent;
class USoundCue;
class USpringArmComponent;
class UUserwidget;
struct FTimerHandle;
struct FMapAnnotation;

//delegate declaration for the player damage
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FPlayerTakeDamageDelegate , float , fDamageValue );

//delegate declaration for the player attack input - to bp mapping
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FPlayerAttackInputDelegate );

//delegate declaration for the player dash - to bp mapping
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerPerformDashDelegate);


//delegate to trigger death state, for updating player in BP
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDiedDelegate);

//delegate to stop particle effect on pickaxe
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FPlayerStopChargedAttack );
UCLASS(config=Game)
class AEchoesCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEchoesCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Event method fired when the player lands from falling
	// Param1: The hit result containing the data where the character landed
	virtual void Landed( const FHitResult& krsHit ) override;

	UFUNCTION(BlueprintCallable)
	void Reset();

	/** Allow other UObject's tied to this Character use UFUNCTION(exec) - this supresses other UFUNCTION(exec) calls on this class it seems.*/
	//virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

	UFUNCTION( BlueprintCallable, Category = "StopDash" )
	void StopDash();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float m_fBaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float m_fBaseLookUpRate;

	// Use tick to call camera tracing
	virtual void Tick(float fDeltaTime) override;

	//delegate to trigger an event in blueprint when the player gets the damage
	UPROPERTY( BlueprintAssignable, Category = "Player Damage" )
	FPlayerTakeDamageDelegate m_dPlayerTakeDamageDelegate;

	//Delegate to trigger an event in blueprint when the player presses the attack input binding
	UPROPERTY( BlueprintAssignable, Category = "Player Input" )
	FPlayerAttackInputDelegate m_dPlayerAttackInputDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Player Death")
	FPlayerDiedDelegate m_dPlayerDeathDelegate;

	// Function that allows the damage to take damage and process it accordingly
	virtual float TakeDamage( float fDamageAmount, struct FDamageEvent const&  rsDamageEvent, 
	AController* pcEventInstigator, AActor* pcDamageCauser ) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Testbed", DisplayName = "Camera World Rotation")
	FRotator m_sCameraWorldRot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Testbed", DisplayName = "Spring Arm Length")
	float m_fSpringArmLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Testbed", DisplayName = "Camera FOV")
	float m_fCameraFOV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Testbed", DisplayName = "Camera Locked")
	bool m_bIsCameraLocked;

	//Camera shake component
	UPROPERTY ( EditAnywhere, Category = "Camera Shake" , DisplayName = "Camera Shake Component" )
	TSubclassOf<UPlayerCameraShake> m_pcPlayerCameraShake;

	//The max amount of shake at which the camera will shake
	UPROPERTY ( EditAnywhere , Category = "Camera Shake" , DisplayName = "Max Shake Scale" )
	float m_fMaxShakeScale;

	//The amount by which the shake scale will increase each frame during shaking
	UPROPERTY ( EditAnywhere , Category = "Camera Shake" , DisplayName = "Shake Scale multiplier" )
	float m_fShakeScaleMultiplier;

	//Hold duration after which the camera will start shaking
	UPROPERTY ( EditAnywhere , Category = "Camera Shake" , DisplayName = "Hold Duration Before Shaking" )
	float m_fHoldDurationBeforeShaking;

	//======== Command-Line Functions =========

	UFUNCTION(exec)
	void SetControlRotation(float fP, float fY, float fR);

	UFUNCTION(exec)
	void SetFOV(float fValue);

	UFUNCTION(exec)
	void SetArmDistance(float fValue);

	UFUNCTION(exec)
	void LockCamera();

	//=========================================

	void ChangeFOV(float fValue);
	void ChangeDistance(float fValue);

	//Getter for camera rotation about the actor
	FRotator GetCameraRotation() { return GetControlRotation(); };

	//Getter for spring arm length between the camera and the actor
	float GetSpringArmLength();

	//Getter for field of view value from the attached camera on the character.
	float GetCameraFOV();

	void ShakeCamera();

	// Set the respawn location of the character
	// Param1: The location that the player should be respawn at
	void SetRespawnLocation( FVector sRespawnLocation ) { m_sRespawnLocation = sRespawnLocation; };

	// Set the respawn location of the player to its current location
	void SetRespawnLocation() { m_sRespawnLocation = GetActorLocation(); };

	//Return value of m_bIsCameraLocked and output log message if it is locked.
	bool CameraLockCheck();

	// Sets whether the player can use double jump or not
	// Param1: Set to true if the player can double jump, false if not
	void SetDoubleJump( bool bDoubleJump ) { m_bCanDoubleJump = bDoubleJump; };

	// Sets whether the player has unlocked the dash ability
	void SetDashEnabled();

	//method to check if player requested for next combo attack
	void CheckForNextAttack ( );

// ======================================================================== COMBAT FUNCTIONS / PROPERTIES ========================================================================

	// Make the player 'stunned'
	// Zero out player velocity, Disable input for a period (except grapple-input), Freeze animation & reset attack variables
	// @Param1: Stun Duration
	// @Param2: Is this be a timed stun or a input threshold stun? - if input then duration is ignored
	void BecomeStunned( float fStunDuration, bool bIsInputDelimited = false );

	// Simple check function to see if the stun has been escaped (grapple over)
	void CheckStunEnded();

	// Sets whether the player has unlocked the charged attack ability
	void SetChargedAttackEnabled();

	//Is the player currently attacking?
	bool IsPlayerAttacking() { return m_bIsAttacking; }

	// Use to play the animation of the chest opening
	void SetOpeningChestAnimState(bool bState);


	// ======================================================================== SOUNDS ========================================================================

	// A variable for holding the light attack sounds
	UPROPERTY ( BlueprintReadOnly , EditAnywhere , Category = "Sound" , DisplayName = "Light Attack Sound" )
	USoundCue* m_pcLightAttackSound;

	// A variable for holding the heavy attack sound
	UPROPERTY ( BlueprintReadOnly , EditAnywhere , Category = "Sound" , DisplayName = "Heavy Attack Sound" )
	USoundCue* m_pcHeavyAttackSound;

	//Single jump sound
	UPROPERTY ( EditAnywhere , Category = "Sound" , DisplayName = "Single Jump" )
	USoundCue* m_pcSingleJumpSound;

	//double jump sound
	UPROPERTY ( EditAnywhere , Category = "Sound" , DisplayName = "Double Jump" )
	USoundCue* m_pcDoubleJumpSound;

	// A variable for holding the flinch sounds
	UPROPERTY ( EditAnywhere , Category = "Sound" , DisplayName = "Flinch Sound" )
	USoundCue* m_apcFlinchSound;

	// A variable for holding the death sounds
	UPROPERTY ( EditAnywhere , Category = "Sound" , DisplayName = "Death Sound" )
	USoundCue* m_pcDeathSound;

	// A variable for holding the footstep sounds
	UPROPERTY ( EditAnywhere , Category = "Sound" , DisplayName = "Footstep Sound" )
	USoundCue* m_pcFootStepSound;

	//map update sound
	UPROPERTY ( EditAnywhere , Category = "Sound" , DisplayName = "Map update sound" )
	USoundCue* m_pcMapUpdateSound;

	//widget popup sound
	UPROPERTY ( EditAnywhere , Category = "Sound" , DisplayName = "Widget popup sound" )
	USoundCue* m_pcWidgetPopupSound;

	// ======================================================================== SOUNDS END ========================================================================

protected:
	//bool to check if we are measuring pressed duration of melee button
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Melee Attack", DisplayName = "Is Melee Attack pressed" )
	bool m_bIsChargedAttackPressed;

	//boolean to check whether player has charged attack ability
	UPROPERTY( BlueprintReadOnly )
	bool m_bChargedAttackAcquired;

	//current melee pressed duration. duration for which the melee button was pressed
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Melee Attack", DisplayName = "Melee Pressed Duration" )
	float m_fCurrentMeleePressedDuration;

	//threshold duration for heavy attack. duration after which player will hit heavy melee
	UPROPERTY( EditAnywhere, Category = "Melee Attack", DisplayName = "Threshold Duration for Heavy melee" )
	float m_fThresholdDurationForHeavyMelee;

	//variable to check if the last attack was charged attack, this will be used to destroy the crystal in one shot
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	bool m_bIsLastAttackWasChargedAttack;

	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Credits Widget" )
	TSubclassOf<UUserWidget> m_pcCreditsWidget;

	//delegate for stoppping particle effects on pickaxe
	UPROPERTY ( BlueprintAssignable , Category = "Player Attack" )
	FPlayerStopChargedAttack m_dPlayerStopChargedDelegate;

	//variable to handle the current attack window duration
	UPROPERTY( BlueprintReadOnly, Category = "Melee Attack", DisplayName = "Current Attack Window Duration" )
	float m_fAttackWindowCurrentTime;

	// Function called by the player control scheme, starts a timer,
	// if timer runs out before pressed button is pressed the timer runs out then call single click function below.
	void SingleClickAttack();

	void ChargedAttack ( );

	// to start detecting the time for which the melee attack was pressed
	void StartChargedAttackPressedDurationMeasuring();

	// to start detecting the time for which the melee attack was pressed
	void StopChargedAttackPressedDurationMeasuring();

	// Function that is called when pickaxe touches an enemy
	UFUNCTION()
	void OnPickAxeOverlap( UPrimitiveComponent* pcHitComponent, AActor* pcOtherActor,
	UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex, bool bFromSweep, const FHitResult & rSweepResult );

	// Variable to check against to prevent movement/attack/dash/jump inputs (aka player is unable to move yet we want to be able to process input)
	UPROPERTY( BlueprintReadWrite )
	bool m_bIsPlayerTrapped;

	//Variable used for attack checks, is the player currently attacking
	UPROPERTY( BlueprintReadWrite )
	bool m_bIsAttacking;

	//Variable used for attack checks, was the next attack queued?
	UPROPERTY( BlueprintReadWrite )
	bool m_bTriggerNextAttack;

	//Variable used for attack checks, is the player attack window open? (anim-notify controlled)
	UPROPERTY( BlueprintReadWrite )
	bool m_bIsNextAttackWindowOpen;

	//Variable used for which combo attack is currently running
	UPROPERTY ( BlueprintReadWrite )
	int m_iComboAttackCount;

private:

	//Player melee attack particle emitter
	UPROPERTY( EditAnywhere, Category = "Melee Attack | VFX" )
	UParticleSystem* m_pcMeleePatricleEmitter1;

	//Player melee attack particle emitter
	UPROPERTY( EditAnywhere, Category = "Melee Attack | VFX" )
	UParticleSystem* m_pcMeleePatricleEmitter2;

	//Player melee attack particle emitter
	UPROPERTY( EditAnywhere, Category = "Melee Attack | VFX" )
	UParticleSystem* m_pcMeleePatricleEmitter3;

	UPROPERTY( Transient )
	UUserWidget* m_pcCreditsWidgetInst;

	// enum of the player attack Animation deceleration 
	enum EComboAttack
	{
		ComboAttackOne = 1,
		ComboAttackTwo,
		ComboAttackThree
	};

	// Storage of the current combo attack iterator
	EComboAttack m_eComboAttackEnum;

	//Attack callback for use by the attack delegate
	void CBAttackCall();

	// The class of encasement actor that spawns when successfully trapping the player
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values | Boss" )
	TSubclassOf<AActor> m_pcStunEncasementActorClass;

	// The encasement actor that spawns when successfully trapping the player
	UPROPERTY( Transient )
	AActor* m_pcStunEncasementActorInst;

// =================================================================================================================================================================================

public:

	//Player stats component
	UPROPERTY( VisibleDefaultsOnly, BlueprintReadOnly )
	UPlayerStatsComponent* m_pcPlayerStatsComponent;

protected:

	UMaterialInstanceDynamic * m_pcDynamicOverlay;

	//Hound Grapple component
	UPROPERTY( VisibleDefaultsOnly, BlueprintReadOnly )
	UHoundGrappleReciever* m_pcHoundGrappleReciever;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);
	
	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// Method called when input threshold reached or timer handle callback following BecomeStunned().
	void CBStunRelease();

	// Starts a countdown timer to check for double click input before executing normal dash
	UFUNCTION( Blueprintcallable )
	void CBDash();

	// Resets the ability for the player to dash after cooldown
	void CBResetDash();

	// Callback function for stamina upgrade pressed
	void CBEnergyUpgradePressed();

	// Callback function for stamina upgrade released
	void CBEnergyUpgradeReleased();

	// Callback function for health upgrade pressed
	void CBHealthUpgradePressed();

	// Callback function for health upgrade released
	void CBHealthUpgradeReleased();

	// Callback function to upgrade max energy
	void CBUpgradeMaxEnergy();

	// Callback function to upgrade max health
	void CBUpgradeMaxHealth();

	// Function to actually make the player dash forward in their facing direction
	// Param1: The force to dash forward with
	UFUNCTION( BlueprintCallable )
	void DashForward( );

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// End of APawn interface
private:

	//The location that the player will respawn at if they die
	FVector m_sRespawnLocation;

	// Bool to track if the player has unlocked double jump
	bool m_bCanDoubleJump;

	// Bool to track if the player is able to dash at the current time
	// Need it exposed for sash ghost effect
	UPROPERTY( BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	bool m_bCanDash;

	// Boolean to track how long the stamina upgrade button is held
	bool m_bEnergyUpgrade;

	// Boolean to track how long the health upgrade button is held
	bool m_bHealthUpgrade;

	// Track what phase of the jump cycle the character is in
	int m_iCurrentJump;

	// The timer handle used to create a callback for single/double dash
	FTimerHandle m_sDashTimer;

	// The timer handle used to upgrade health/energy
	FTimerHandle m_sUpgradeTimer;

	// The timer handle for the stun release callbacks
	FTimerHandle m_sStunCBTimer;

	//Has the dash ability been acquired? Needed for the dash Ghost Effect
	UPROPERTY( BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	bool m_bDashAquired;

	AMenuWidgetPlayerController* m_pcMenuWidgetController;

	// Reference to Input Capability
	UInputComponent*			m_pcInput;

	// enum of the player states deceleration 
	enum EPlayerState
	{
		Idle,
		Running,
		Attacking,
		Jumping,
		Dashing,
		Blocking
	};
	
	//time variables used for PostProcessing Effects
	int m_iHealthLowTimeCounter;

	//border values for special effect
	UPROPERTY( EditAnywhere, Category = "VFX", DisplayName = "Low Health Warning Limit" )
	float m_fLowHealWarnLimit;

	UPROPERTY( EditDefaultsOnly, Category = "VFX", DisplayName = "Low Health PostProcess Material"  )
	UMaterialInterface* m_pcLowHealthPPMat;

	// Variable used to determine if the jump bindings need to be restored
	bool m_bIsJumpBound;

	// enum for current state of player
	EPlayerState m_eCurrentPlayerState;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, DisplayName = "CameraBoom", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* m_pcCameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, DisplayName = "FollowCamera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* m_pcFollowCamera;

	//Dash Ghost Creation Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VFX, DisplayName = "Ghost Component", meta = (AllowPrivateAccess = "true"))
	class UDashGhostComponent* m_pcDashGhostComp;

	// The location the player was spawned in
	FVector m_vStartingLocation;

	// Function called by the player control scheme, starts a timer,
	// if timer runs out before pressed button is pressed the timer runs out then call single click function below.
	void SingleClickJump();

	// Called when the player presses the input for the jump action
	void CBJump();
		
	// players jump reset timer handle
	FTimerHandle m_fthJumpCheckTimeHandle;

	// Use this to create the appropriate logic for User Focus
	int m_iCurrentPauseMenuIndex;

	// Use this to determine if the index of the Puase Menu has changed recently
	bool m_bChangedPauseIndex;

	// Checks if the binding for Death needs to be removed from the player 
	bool m_bRemoveDeathBinding;

	// Functionality to enable when the Player has died
	UFUNCTION( BlueprintCallable )
	void DeathInput();

	// The actual instance of the userwidget created and added to viewport
	UPROPERTY( Transient )
	UUserWidget* m_pcDeathWidgetInstance;

	// The Death popup widget that will be displayed on the screen
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Death Popup" )
	TSubclassOf<UUserWidget> m_pcDeathWidget;

private:

	// Define enum for all abilities
	// If referenced like this, flexible and always tied to correct values
	enum EAbilityList
	{
		DOUBLE_JUMP,
		DASH,
		CHARGE_ATTACK
	};

	//PostProcess Material
	UPROPERTY(EditDefaultsOnly, Category = "EndGame", DisplayName = "Black Screen")
	UMaterial* m_pcBlackScreenPPMat;

	float m_ffadecounter;

	bool m_bGameFinish;

	int m_iStage;

	ANPC* m_pcNPC;

	//NPC Actor
	UPROPERTY(EditDefaultsOnly, Category = "EndGame", DisplayName = "NPC BP")
	TSubclassOf<ANPC> m_pcNpc_BP;

	//Set to < than 1 to delay the fade out.
	UPROPERTY(EditDefaultsOnly, Category = "EndGame", DisplayName = "Fade Dilation")
	float m_fFadeTimeDilation;

public:

	// The amount of cooldown time on the player dash ability
	UPROPERTY( EditAnywhere, Category = "Designer Values|Dash", DisplayName = "Dash Cooldown" )
	float m_fDashCooldown;

	// The distance to which the player will move on dash
	UPROPERTY ( BlueprintReadOnly, EditAnywhere , Category = "Designer Values|Dash" , DisplayName = "Dash Distance" )
	float m_fDashDistance;

	//variable to save the start point of the dash
	UPROPERTY ( BlueprintReadWrite )
	FVector m_vDashStartPoint;

	//variable to calculate and save the final point after player dash
	UPROPERTY ( BlueprintReadWrite )
	FVector m_vDashEndPoint;

	//Delegate to trigger an event in blueprint when the player dash
	UPROPERTY ( BlueprintAssignable , Category = "Player Dash" )
	FPlayerPerformDashDelegate m_dPlayerPerformDashDelegate;

	// Time before player can jump again
	UPROPERTY( EditAnywhere, Category = "Controls" )
	float m_fJumpCheckTime;

	// The amount of time to hold an upgrade button before upgrade happens
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Upgrades", DisplayName = "Upgrade Time" )
	float m_fUpgradeTime;

	EPlayerState	GetCurrentState()			{ return m_eCurrentPlayerState; }

	//////////////////////////////////////////////////////////////////////////

	// Use to get the current index of the pause menu
	int				GetCurrentPauseIndex			()					{ return m_iCurrentPauseMenuIndex; };

	// Setter for m_iCurrentPauseMenuIndex
	void			SetCurrentPauseIndex			( int iIndex )		{ m_iCurrentPauseMenuIndex = iIndex; };

	// Getter for if the index has recently changed
	bool			GetChangedPauseIndex			()					{ return m_bChangedPauseIndex; };

	// Setter for if the index has recently changed
	void			SetChangedPauseIndex			( bool bChange )	{ m_bChangedPauseIndex = bChange; };

	//////////////////////////////////////////////////////////////////////////

	// Get the current annotation array at runtime
	//TArray<FMapAnnotation> GetAnnotationArray() { return m_asAnnotationDataArray; }

	//////////////////////////////////////////////////////////////////////////
	// Functionality relating to the Map Annotation & Upgrade UI Systems
	// Getters for each ability status such as GetCanDash
	// Deterministic logic for Annotation in SetMapAnnotationVisibility
	//////////////////////////////////////////////////////////////////////////

	// Check if the player can the player Double Jump
	bool			GetCanDoubleJump				()	{ return m_bCanDoubleJump; };

	// Check if the player can Dash
	bool			GetCanDash						()	{ return m_bDashAquired; };

	UFUNCTION(BlueprintCallable)
	inline UPlayerStatsComponent* GetStatsComponent()  
	{ 
		return m_pcPlayerStatsComponent; 
	}

	void PlayFootPrintSound ( );

	UFUNCTION(BlueprintCallable)
	void EndGame();

	bool IsLastAttackWasChargedAttack ( ) { return m_bIsLastAttackWasChargedAttack; }

	void PlayMapUpdateSound ( );

	void PlayWidgetPopupSound ( );
};