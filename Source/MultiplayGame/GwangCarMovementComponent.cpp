// Fill out your copyright notice in the Description page of Project Settings.


#include "GwangCarMovementComponent.h"
#include <DrawDebugHelpers.h>

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

	//if (GetOwnerRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	//{
	//	LastMove = CreateMove(DeltaTime);
	//	SimulateMove(LastMove);
	//}

	bool IsClient = GetOwnerRole() == ROLE_AutonomousProxy;
	bool IsHost = GetOwnerRole() == ROLE_Authority && Cast<APawn>(GetOwner())->IsLocallyControlled();

	if (IsClient || IsHost)
	{
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}
}

FVehicleMove UGwangCarMovementComponent::CreateMove(float DeltaTime)
{
	FVehicleMove Move;
	Move.DeltaTime = DeltaTime;
	Move.Throttle = Throttle;
	Move.SteeringThrow = SteeringThrow;
	Move.Drift = Drift;
	Move.Time = GetWorld()->TimeSeconds;

	return Move;
}

void UGwangCarMovementComponent::SimulateMove(const FVehicleMove& Move)
{
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;
	Force += GetAirResistance();	// Air Resistance = Speed^2 * DragCoefficient
	Force += GetRollingResistance();	// Rolling Resistance = NormalForce(Mass*Gravity) * RollingResistanceCoefficient
	Force += GetSideFriction();	// dot(velocity, normalright) * -normalright * mass

	if (IsVerlet)
	{
		UpdateLocationFromVelocity(Move.DeltaTime);
		FVector NewAcceleration = Force / MassInKilo;
		Velocity += (Acceleration + NewAcceleration) * (Move.DeltaTime * 0.5);
		Acceleration = NewAcceleration;
	}
	else // Euler
	{
		UpdateLocationFromVelocity(Move.DeltaTime);
		Acceleration = Force / MassInKilo;
		Velocity += (Acceleration * Move.DeltaTime);
	}

	ApplyRotation(Move.DeltaTime, Move.SteeringThrow, Move.Drift);
}

void UGwangCarMovementComponent::ApplyRotation(float DeltaTime, float InSteeringThrow, bool InDrift)
{
	float DeltaX = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaX / (InDrift ? MinTurningRadius / 3 : MinTurningRadius) * InSteeringThrow;	// dθ = dx / radius
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

	CurrentRotationAngle = FMath::LerpStable(CurrentRotationAngle, RotationAngle, SteerLerp * DeltaTime);

	if (!InDrift)
	{
		Velocity = RotationDelta.RotateVector(Velocity);
	}
	else
	{
		FQuat DriftRotationDelta(GetOwner()->GetActorUpVector(), CurrentRotationAngle);
		Velocity = DriftRotationDelta.RotateVector(Velocity);
	}

	GetOwner()->AddActorWorldRotation(RotationDelta, true);
}

void UGwangCarMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	float M2cm = 100; // Meter to centimeter
	FVector DeltaX;

	if (IsVerlet)
	{
		DeltaX = (Velocity * DeltaTime * M2cm) + (Acceleration * DeltaTime * DeltaTime * 0.5);
	}
	else // Euler
	{
		DeltaX = Velocity * DeltaTime * M2cm;
	}

	FHitResult Hit;
	GetOwner()->AddActorWorldOffset(DeltaX, true, &Hit);
	if (Hit.IsValidBlockingHit())
	{
		HandleCollision(Hit);
	}
}

void UGwangCarMovementComponent::HandleCollision(FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	if (HitActor == nullptr)
	{
		return;
	}

	FVector V1i = Velocity;
	float M1 = MassInKilo;

	UGwangCarMovementComponent* MoveComp = nullptr;
	MoveComp = HitActor->FindComponentByClass<UGwangCarMovementComponent>();
	FVector V2i = MoveComp != nullptr ? MoveComp->GetVelocity() : HitActor->GetVelocity();
	float M2 = 99999;
	if (MoveComp != nullptr)
	{
		M2 = MoveComp->GetMass();
	}
	else if (HitActor->GetRootComponent()->IsSimulatingPhysics() && HitActor->IsRootComponentMovable())
	{
		M2 = Hit.Component->GetMass();
	}

	// this vehicle
	FVector HitDir1 = GetOwner()->GetActorLocation() - Hit.ImpactPoint;
	HitDir1.Z = 0;
	FVector V1f = V1i - ((M2 + M2) / (M1 + M2)) * FVector::DotProduct((V1i - V2i), HitDir1) / (HitDir1.Size() * HitDir1.Size()) * HitDir1;
	this->Velocity = V1f * this->Elast;

	// other
	FVector HitDir2 = Hit.ImpactPoint - GetOwner()->GetActorLocation();
	HitDir2.Z = 0;
	if (MoveComp != nullptr)
	{
		FVector V2f = V2i - ((M1 + M1) / (M1 + M2)) * FVector::DotProduct((V2i - V1i), HitDir2) / (HitDir2.Size() * HitDir2.Size()) * HitDir2;
		MoveComp->SetVelocity(V2f * MoveComp->GetElast());
	}

#pragma region 1D Collision
	// V1i + V1f = V2i + V2f
	// 1. V2f = V1i + V1f - V2i

	// Momentum( before collision = after collision )
	// 2. V1i * M1 + V2i * M2 = (V1f * M1 + V2f * M2) * elast

	// 1 + 2		
	// V1f = (V1i * (M1 - M2) + V2i * (M2 + M2)) / (M1 + M2)
	// V2f = (V2i * (M2 - M1) + V1i * (M1 + M1)) / (M1 + M2)

	// Elast
	//float Elast = 0;
	//FVector V1f = (Elast * M2 * (V2i - V1i) + M1 * V1i + M2* V2i) / (M1 + M2);
	//FVector V2f = (Elast * M1 * (V1i - V2i) + M1 * V1i + M2* V2i) / (M1 + M2);
#pragma endregion
#pragma region 2D Collision
//float RadianToDegree = 180 / PI;

		//FVector V1i = Velocity;
		//float Ang1 = FMath::Atan2(V1i.Y, V1i.X) * RadianToDegree;
		//float M1 = MassInKilo;

		//UGwangCarMovementComponent* MoveComp = Hit.Actor->FindComponentByClass<UGwangCarMovementComponent>();
		//FVector V2i = MoveComp != nullptr ? MoveComp->GetVelocity() : Hit.Actor->GetVelocity();
		//float Ang2 = FMath::Atan2(V2i.Y, V2i.X) * RadianToDegree;
		//float M2 = 99999;
		//if (MoveComp != nullptr)
		//{
		//	M2 = MoveComp->GetMass();
		//}
		//else if (Hit.Actor->IsRootComponentMovable())
		//{
		//	M2 = Hit.Component->GetMass();
		//}

		//FVector ContactDir = (Hit.ImpactPoint - GetOwner()->GetActorLocation()).GetSafeNormal();
		//float Phi = FMath::Atan2(ContactDir.Y, ContactDir.X) * RadianToDegree;

		//UE_LOG(LogTemp, Warning, TEXT("Ang1 45?: %f"), Ang1);
		//UE_LOG(LogTemp, Warning, TEXT("Ang2 0?: %f"), Ang2);
		//UE_LOG(LogTemp, Warning, TEXT("Phi 90?: %f"), Phi);

		//// Rotate
		//FVector V1xr = V1i * FMath::Cos(Ang1 - Phi);
		//FVector V1yr = V1i * FMath::Sin(Ang1 - Phi);
		//FVector V2xr = V2i * FMath::Cos(Ang2 - Phi);
		//FVector V2yr = V2i * FMath::Sin(Ang2 - Phi);

		//// Calculate
		//FVector V1fxr = (V1xr * (M1 - M2) + V2xr * (M2 + M2)) / (M1 + M2);
		//FVector V1fyr = V1yr;

		//// Rotate back
		//FVector V1fx = V1fxr * FMath::Cos(Phi) + V1fyr * FMath::Cos(Phi + (PI / 2));
		//FVector V1fy = V1fxr * FMath::Sin(Phi) + V1fyr * FMath::Sin(Phi + (PI / 2));

		//Velocity = V1fx + V1fy;
#pragma endregion
}

FVector UGwangCarMovementComponent::GetAirResistance()	// Air Resistance = Speed^2 * DragCoefficient
{
	float Speed = Velocity.Size();
	return -Velocity.GetSafeNormal() * Speed * Speed * DragCoefficient;
}

FVector UGwangCarMovementComponent::GetRollingResistance()	// Rolling Resistance = NormalForce(Mass*Gravity) * DragCoefficient
{
	float Gravity = -GetWorld()->GetGravityZ() / 100;	// 9.81....
	float NormalForce = MassInKilo * Gravity;	// Normal Force = Mass * Gravity
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

FVector UGwangCarMovementComponent::GetSideFriction()
{
	FVector DotV = GetOwner()->GetActorRightVector() * FVector::DotProduct(Velocity, GetOwner()->GetActorRightVector());
	FVector SideFriction = GetMass() * -DotV;

	if (SideFriction.Size() > MaxSideFriction * GetMass())
	{
		UE_LOG(LogTemp, Warning, TEXT("SideFriction: %f"), SideFriction.Size());
		SideFriction *= MaxSideFriction * GetMass() / SideFriction.Size();
	}
	return SideFriction;
}

