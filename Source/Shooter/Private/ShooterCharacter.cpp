// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter():
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	bAiming(false),
	CameraDefaultFOV(0.f),	// set in BeginPlay()
	CameraZoomFOV(60.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 将相机连接到弹簧臂末端
	FollowCamera->bUsePawnControlRotation = false; // 相对于弹簧臂不旋转

	// 不旋转角色，只旋转相机
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera) {
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;

	}
}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f)) {
		// find out which way is forward
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator RotationYaw{ 0.0, Rotation.Yaw, 0.0 };

		const FVector Dircetion{ FRotationMatrix{RotationYaw}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Dircetion, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f)) {
		// find out which way is right
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator RotationYaw{ 0.0, Rotation.Yaw, 0.0 };

		const FVector Dircetion{ FRotationMatrix{RotationYaw}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Dircetion, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::FireWeapon()
{
	//UE_LOG(LogTemp, Warning, TEXT("Fire Weapon!"))
	if (FireSound) {
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket) {
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(),
			BeamEnd);

		if (bBeamEnd) {
			if (ImpactParticles) {
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform);
			if (Beam) {
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
				
		}

	}
	

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage) {
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool AShooterCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation, 
	FVector& OutBeamLocation)
{
	// 获取当前viewport大小
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// 获取crosshair的屏幕空间位置
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 获取crosshair的世界坐标和世界方向
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld) {
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };

		// 设置beam end点为跟踪线end点
		OutBeamLocation = End;

		// 沿着crosshair世界坐标跟踪
		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit) { // 如果击中
			// beam end点为击中点
			OutBeamLocation = ScreenTraceHit.Location;
		}

		// 执行二次跟踪，本次跟踪始于gun barrel
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector WeaponTraceEnd{ OutBeamLocation };
		GetWorld()->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility);

		if (WeaponTraceHit.bBlockingHit) {
			OutBeamLocation = WeaponTraceHit.Location;
		}
		return true;
	}

	return false;
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
	GetFollowCamera()->SetFieldOfView(CameraZoomFOV);
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
	GetFollowCamera()->SetFieldOfView(CameraDefaultFOV);
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);
}
