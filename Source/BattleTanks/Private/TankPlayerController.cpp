// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "..\Public\TankPlayerController.h"

#define OUT

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto ControlledTank = GetControlledTank();
	if(!ControlledTank)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController not possessing a tank"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController possessing a tank: %s"), *ControlledTank->GetName());
	}
}

void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimTowardsCrosshair();
}

ATank* ATankPlayerController::GetControlledTank() const
{
	return Cast<ATank>(GetPawn());
}

void ATankPlayerController::AimTowardsCrosshair()
{
	if (!GetControlledTank()) return;

	OUT FVector HitLocation;
	if (GetSightRayHitLocation(HitLocation)) //Linetraces as a side effect
	{
		//UE_LOG(LogTemp, Warning, TEXT("HIT_LOCATION: %s"), *HitLocation.ToString());
	}
}


bool ATankPlayerController::GetSightRayHitLocation(OUT FVector& HitLocation) const
{
	//Get Crosshair position in viewport
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewportSizeY * CrosshairYLocation);

	OUT FVector LookDirection;
	if(GetLookDirection(ScreenLocation, LookDirection))
	{
		UE_LOG(LogTemp, Warning, TEXT("Look Direction: %s"), *LookDirection.ToString());
		GetLookVectorHitLocation(LookDirection, HitLocation);
	}

	HitLocation = FVector(1.0);
	return true;
}

bool ATankPlayerController::GetLookDirection(OUT FVector2D ScreenLocation, OUT FVector& LookDirection) const
{
	//Translate crosshair position from viewport to world position ("Deproject")
	OUT FVector CameraWorldLocation;
	return DeprojectScreenPositionToWorld(
		ScreenLocation.X, 
		ScreenLocation.Y, 
		OUT CameraWorldLocation, 
		OUT LookDirection
	);
}

bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, OUT FVector& HitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);
	if(GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation, 
		EndLocation,
		ECollisionChannel::ECC_Visibility
	))
	{
		HitResult.Location;
		return true;
	}

	return false;
}