// Fill out your copyright notice in the Description page of Project Settings.


#include "GwangCarMovementComponent.h"

// Sets default values for this component's properties
UGwangCarMovementComponent::UGwangCarMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGwangCarMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UGwangCarMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVehicleMove UGwangCarMovementComponent::CreateMove(float DeltaTime)
{
	FVehicleMove Move;
	Move.DeltaTime = DeltaTime;
	Move.Throttle = Throttle;
	Move.SteeringThrow = SteeringThrow;
	Move.Time = GetWorld()->TimeSeconds;

	return Move;
}

void UGwangCarMovementComponent::SimulateMove(const FVehicleMove& Move)
{
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;
	Force += GetAirResistance();	// Air Resistance = Speed^2 * DragCoefficient
	Force += GetRollingResistance();	// Rolling Resistance = NormalForce(Mass*Gravity) * DragCoefficient

	FVector Acceleration = Force / MassInKilo;

	Velocity += (Acceleration * Move.DeltaTime);

	ApplyRotation(Move.DeltaTime, Move.SteeringThrow);
	UpdateLocationFromVelocity(Move.DeltaTime);
}

void UGwangCarMovementComponent::ApplyRotation(float DeltaTime, float InSteeringThrow)
{
	//float RotationAngle = MaxDegreesPerSec * DeltaTime * SteeringThrow;
	float DeltaX = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaX / MinTurningRadius * InSteeringThrow;	// dθ = dx / radius
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);
	Velocity = RotationDelta.RotateVector(Velocity);
	GetOwner()->AddActorWorldRotation(RotationDelta, true);
}

void UGwangCarMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector DeltaX = Velocity * DeltaTime * 100; // 100 -> convert meter to centimeter
	FHitResult Hit;
	GetOwner()->AddActorWorldOffset(DeltaX, true, &Hit);
	if (Hit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;

#pragma region Test Stuff
		//AGwangCar* AnotherCar = Cast<AGwangCar>(Hit.Actor);
		//if (AnotherCar != nullptr)
		//{
		//	FVector u1 = Velocity;
		//	float m1 = MassInKilo;

		//	FVector u2 = AnotherCar->GetVehicleMovementComponent()->GetVelocity();
		//	float m2 = AnotherCar->GetVehicleMovementComponent()->GetMass();

		//	FVector v1 = (u1 * (m1 - m2)) / (m1 + m2) + (2 * m2 * u2) / (m1 + m2);
		//	FVector v2 = (2 * m1 * u1) / (m1 + m2) + (u2 * (m2 - m1)) / (m1 + m2);

		//	Velocity = v1;
		//	AnotherCar->GetVehicleMovementComponent()->SetVelocity(v2);
		//}

		//FVector v1i = Velocity;
		//float m1 = MassInKilo;
		//float θ1 = acos(FVector::DotProduct(FVector::ForwardVector, v1i.GetSafeNormal()));
		//UE_LOG(LogTemp, Warning, TEXT("θ1: %f"), θ1 * 180 / PI);
		//UE_LOG(LogTemp, Warning, TEXT("v1: %s"), *v1i.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("m1: %f"), m1);

		//FVector v2i = Hit.GetActor()->GetActorForwardVector();
		//float m2 = Hit.Component->GetMass();
		//float θ2 = acos(FVector::DotProduct(FVector::ForwardVector, v2i.GetSafeNormal()));
		//UE_LOG(LogTemp, Warning, TEXT("v2: %s"), *v2i.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("m2: %f"), m2);

		//float φ = acos(FVector::DotProduct(FVector::ForwardVector, (Hit.Location - GetActorLocation()).GetSafeNormal()));

		//θ1 = θ1 * 180 / PI;
		//θ2 = θ2 * 180 / PI;
		//φ = φ * 180 / PI;
		//UE_LOG(LogTemp, Warning, TEXT("θ1: %f"), θ1);
		//UE_LOG(LogTemp, Warning, TEXT("θ2: %f"), θ2);
		//UE_LOG(LogTemp, Warning, TEXT("φ: %f"), φ);

		//// find the velocities in the new coordinate system
		//FVector v1xr = v1i * FMath::Cos(θ1 - φ);
		//FVector v1yr = v1i * FMath::Sin(θ1 - φ);
		//FVector v2xr = v2i * FMath::Cos(θ2 - φ);
		//FVector v2yr = v2i * FMath::Sin(θ2 - φ);

		//// find the final velocities in the normal reference frame
		//FVector v1fxr = ((m1 - m2) * v1xr + (m2 + m2) * v2xr) / (m1 + m2);
		//FVector v2fxr = ((m1 + m1) * v1xr + (m2 - m1) * v2xr) / (m1 + m2);
		//// the y velocities will not be changed
		//FVector v1fyr = v1yr;
		//FVector v2fyr = v2yr;

		//// convert back to the standard x,y coordinates
		//FVector v1fx = v1fxr * FMath::Cos(φ) + v1fyr * FMath::Cos(φ + PI / 2);
		//FVector v1fy = v1fxr * FMath::Sin(φ) + v1fyr * FMath::Sin(φ + PI / 2);

		//FVector v2fx = v2fxr * FMath::Cos(φ) + v2fyr * FMath::Cos(φ + PI / 2);
		//FVector v2fy = v2fxr * FMath::Sin(φ) + v2fyr * FMath::Sin(φ + PI / 2);

		//Velocity = v1fx + v1fy;
		//UE_LOG(LogTemp, Warning, TEXT("v1f: %s"), *Velocity.ToString());
#pragma endregion

	}
}

FVector UGwangCarMovementComponent::GetAirResistance()
{
	float Speed = Velocity.Size();
	return -Velocity.GetSafeNormal() * Speed * Speed * DragCoefficient;
}

FVector UGwangCarMovementComponent::GetRollingResistance()
{
	float Gravity = -GetWorld()->GetGravityZ() / 100;	// 9.81....
	float NormalForce = MassInKilo * Gravity;	// Normal Force = Mass * Gravity
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}


