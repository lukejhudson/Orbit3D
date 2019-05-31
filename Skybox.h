// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Skybox.generated.h"

UCLASS()
class ORBIT3D_API ASkybox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkybox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	USphereComponent *Base;
	UStaticMeshComponent *SphereMesh;
	UPROPERTY(EditAnywhere)
	class UMaterial* Material;
	FTimerHandle MoveTimerHandle;
	void MoveToPlayer();

};
