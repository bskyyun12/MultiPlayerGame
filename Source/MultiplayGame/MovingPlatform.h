// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "MovingPlatform.generated.h"

USTRUCT()
struct FServerPlatForm
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float ReplicatedTime;
};

UCLASS()
class MULTIPLAYGAME_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Server)
	FServerPlatForm Server;
	UFUNCTION()
	void OnRep_Server();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	FVector StartLocation;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = true))
	FVector TargetLocation;

	UPROPERTY(EditAnywhere)
	float TravalTime = 1;

	UPROPERTY(EditAnywhere)
	float PushingPower = 10;

	float ClientTime;
	FHitResult Hit;

private:
	void SimulateMove(float Time);
	void HandleCollision();

};
