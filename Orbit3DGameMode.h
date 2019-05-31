// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "GameFramework/GameMode.h"
#include "SphereActor.h"
#include "Orbit3DGameMode.generated.h"

UCLASS()
class ORBIT3D_API AOrbit3DGameMode : public AGameMode
{
	GENERATED_BODY()


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AOrbit3DGameMode();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle SpawnTimerHandle;
	void SpawnSphere();
	TArray<ASphereActor*> ActorArray;
	float RunningTime;

	UPROPERTY(EditAnywhere)
	float G = 10.0f;
};



