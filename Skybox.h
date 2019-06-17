#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "Skybox.generated.h"

/*
Actor handling the sky box (or sky sphere) of the game.
*/
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
	// Sphere mesh and root component of the actor
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *SphereMesh;
	// Material/texture of the mesh (may be altered in the editor)
	UPROPERTY(EditAnywhere)
	class UMaterial* Material;
	// Handles timer to call MoveToPlayer at fixed time intervals
	FTimerHandle MoveTimerHandle;
	// Moves the root component to the player's location
	void MoveToPlayer();

};
