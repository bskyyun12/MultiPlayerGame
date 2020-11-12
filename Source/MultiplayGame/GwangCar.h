// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "GwangCarMovementComponent.h"
#include "GwangCarMoveReplicationComponent.h"

#include "GwangCar.generated.h"

UCLASS()
class MULTIPLAYGAME_API AGwangCar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGwangCar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UGwangCarMovementComponent* GetVehicleMovementComponent() { return MovementComponent; }

private:
	
	void Client_MoveForward(float Value);
	void Client_MoveRight(float Value);

	// Drift
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDriftDelegate);
	UPROPERTY(BlueprintAssignable)
	FDriftDelegate OnDrift;
	UPROPERTY(BlueprintAssignable)
	FDriftDelegate OnStopDrift;
	void DriftPressed();
	void DriftReleased();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UGwangCarMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UGwangCarMoveReplicationComponent* MoveReplicationComponent;
};
