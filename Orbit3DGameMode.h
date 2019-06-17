#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "GameFramework/GameMode.h"
#include "SphereActor.h"
#include "Orbit3DGameMode.generated.h"

/*
Game mode to handle overall game logic as well as gravity calculations between bodies.
*/
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
	// Handles timer to call SpawnSphere at fixed time intervals
	FTimerHandle SpawnTimerHandle;
	// Spawns a single sphere in a random location
	void SpawnSphere();
	// Holds all spheres created by SpawnSphere()
	TArray<ASphereActor*> ActorArray;
	// Tracks the time since the game began
	float RunningTime;

	// Calc position and velocity for NewSphere being spawned in orbit around the given central sphere
	void CalculateOrbitVelocity(ASphereActor *NewSphere, ASphereActor *Central, float MinOrbitDistance, float MaxOrbitDistance);
	// Spawns a planetary system with the centre being the given ASphereActor
	void SpawnPlanetarySystem(ASphereActor *Central);
	// Spawns a planetary system with the centre at the given position Pos and velocity Vel
	void SpawnPlanetarySystem(FVector Pos, FVector Vel);

	// Gravitational constant (can be altered in the editor)
	UPROPERTY(EditAnywhere)
	float G = 2.0f;
};



