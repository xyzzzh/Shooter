// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Shooter/AmmoType.h"
#include "ShooterCharacter.generated.h"

class AAmmo;
class UCameraComponent;
class USpringArmComponent;
class USoundCue;
class UParticleSystem;
class UAnimMontage;


UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called for forward input
	void MoveForward(float Value);

	// Called for side to side input
	void MoveRight(float Value);

	// called via input to turn at a given rate
	void TurnAtRate(float Rate);

	// called via input to look up/down at a given rate
	void LookUpAtRate(float Rate);

	// rotate controller based on mouse X movement
	// @param Value: the input value from mouse movement
	void Turn(float Value);

	// rotate controller based on mouse Y movement
	// @param Value: the input value from mouse movement
	void LookUp(float Value);

	// called when fire button is pressed
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	//set bAiming to true or false with button press
	void AimingButtonPressed();
	void AimingButtonReleased();

	//set current camera field of view
	void CameraInterpZoom(float DeltaTime);

	// set base turn & lookup rate
	void SetLookRates();

	void CalculateCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	UFUNCTION()
	//line trace for items under the crosshairs
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	//trace for items if OverlappedItemCount > 0
	void TraceForItems();

	//spawn a default weapon and equip it
	class AWeapon* SpawnDefaultWeapon();

	//takes a weapon and attach it to the mesh
	void EquipWeapon(AWeapon* WeaponToEquip);

	//detach weapon and let it fall to the ground
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	//drops currently equipped weapon and equips TraceItemWeapon
	void SwapWeapon(AWeapon* WeaponToSwap);

	void InitializeAmmoMap();

	// check to make sure weapon has ammo
	bool WeaponHasAmmo();

	// fireweapon func
	void PlayFireSound();
	void SendBullet();
	void PlayGunFireMontage();

	// bound to the R key and Gamepad Face Button Left 
	void ReloadButtonPressed();

	// Handle reloading of the weapon
	void ReloadWeapon();

	// checks to see if we have ammo of the EquippedWeapon's Ammo type
	bool CarryAmmo();

	// called from anim blueprint with Grad Clip notify 
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	// called from anim blueprint with Grad Clip notify 
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void PickupAmmo(AAmmo* Ammo);

	void InitializeInterpLocation();

	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess= "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess= "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash;

	/* Montage for firing weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HipFireMontage;

	/* Particles spawned upon bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	/* Smoke trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	//True when aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	//Default camera field of view value
	float CameraDefaultFOV;

	//Field of view value for when zoom in
	float CameraZoomFOV;

	//current field of view frame
	float CameraCurrentFOV;

	//interp speed for zooming when aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	//determines the spread of the crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	//velocity component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	//in air component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	//aiming component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	//shooting component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	float ShooterTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShooterTimer;

	//left mouse button or right console trigger pressed
	bool bFireButtonPressed;

	//true when we can fire. false when waiting for the timer
	bool bShouldFire;

	//rate of automatic gun fire
	float AutomaticFireRate;

	//sets a timer between gunshots
	FTimerHandle AutoFireTimer;

	//true if we should trace every frame for items
	bool bShouldTraceForItems;

	//count of overlapped AItems
	int8 OverlappedItemCount;

	//the item we hit last frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	//currently equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	//set this in blueprints for the default weapon class
	//this class or any class drived from this class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	//the item currently hit by our trace in TraceForItem() (could be nullptr)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	//distance outward from the camera for the interp destination
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;

	//distance upward from the camera for the interp destination
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	//map to keep track of ammo of the different ammo types
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	// combat state, can only fire or reload if unoccupied
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	/* Montage for reload animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	/* Montage for equip animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	//transform of the clip when we first grab the clip during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	//scene component to attach to the Character's hand during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp5;

	// array of interp location structs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	const int32 INVENTORY_CAPACITY{6};

	// Delegate for sending slot information to InventoryBar when equipping
	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	//add/subtracts to/from OverlappedItemCount and update bShouldTracForItems
	void IncrementOverlappedItemCount(int8 Amount);

	FVector GetCameraInterpLocation();

	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	FORCEINLINE FInterpLocation GetInterpLocation(int32 Index);
	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	int32 GetInterpLocationIndex();
	void IncrementInterpLocItemCount(int32 Index, int32 Amount);
};
