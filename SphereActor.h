// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "SphereActor.generated.h"

UCLASS()
class ORBIT3D_API ASphereActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASphereActor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetScale(float Scale);
	FVector GetVelocity();
	void SetVelocity(FVector Vec);
	void IncrementVelocity(FVector Vec);
	float GetMass();
	void SetMass(float M);

private:
	USphereComponent *box;
	UStaticMeshComponent *SphereMesh;

	UPROPERTY(EditAnywhere)
	FVector Velocity;

	UPROPERTY(EditAnywhere)
	float Mass;

};
