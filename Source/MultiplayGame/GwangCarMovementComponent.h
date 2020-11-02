// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GwangCarMovementComponent.generated.h"

USTRUCT()
struct FVehicleMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle;
	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;
	UPROPERTY()
	float Time;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYGAME_API UGwangCarMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGwangCarMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	float GetMass() { return MassInKilo; }

	FVector GetVelocity() { return Velocity; }
	void SetVelocity(FVector InVelocity) { Velocity = InVelocity; }
	void SetThrottle(float Value) { Throttle = Value; }
	void SetSteeringThrow(float Value) { SteeringThrow = Value; }

	FVehicleMove CreateMove(float DeltaTime);
	void SimulateMove(const FVehicleMove& Move);

private:
	FVector GetAirResistance();
	FVector GetRollingResistance();

	void ApplyRotation(float DeltaTime, float SteeringThrow);
	void UpdateLocationFromVelocity(float DeltaTime);

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Mass of this vehicle (kg)"))
	float MassInKilo = 1000;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Force applied when the throttle is fully down (N)"))
	float MaxDrivingForce = 10000;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Minimum radius of the car turning circle at full lock (m)"))
	float MinTurningRadius = 10;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Higher is more drag (kg/m)"))
	float DragCoefficient = 16;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Higher is more rolling resistance (kg/m)"))
	float RollingResistanceCoefficient = 0.015;

	FVector Velocity;

	float Throttle;
	float SteeringThrow;
};
