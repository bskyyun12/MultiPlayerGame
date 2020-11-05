// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "MovingPlatform.generated.h"

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

	void OnTriggerActivated();
	void OnTriggerDeactivated();

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(Replicated)
	float Time;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedTranform)
	FTransform ReplicatedTranform;

	UFUNCTION()
	void OnRep_ReplicatedTranform();

	UPROPERTY(EditAnywhere)
	float TravalTime = 1;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = true))
	FVector TargetLocation;

	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	int CurrentNumOfTriggers = 0;

	UPROPERTY(EditAnywhere)
	int RequiredNumOfTriggers = 1;
};
