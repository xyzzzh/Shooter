// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Shooter/AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()

public:
	AAmmo();
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	// override of this func so we can set AmmoMesh properties
	virtual void SetItemProperties(EItemState State) override;

	//called when overlapping area sphere
	UFUNCTION()
	void AmmoSphereOverlap(
		UPrimitiveComponent* OverlapedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:
	// mesh for the ammo pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	// texture for ammo icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture;

	// overlap sphere for picking up the ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	USphereComponent* AmmoCollisionSphere;

public:
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;
};
