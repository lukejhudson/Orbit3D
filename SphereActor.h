#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/Material.h"
#include "SphereActor.generated.h"

/*
Actor for a moveable sphere object.
*/
UCLASS()
class ORBIT3D_API ASphereActor : public AActor
{
	GENERATED_BODY()
	
public:	

	enum SphereType 
	{
		Sun = 0,
		Planet = 1,
		Asteroid = 2
	};

	// Sets default values for this actor's properties
	ASphereActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Get current scale
	float GetScale();
	// Set scale/size of sphere
	void SetScale(float Scale);
	// Get current velocity
	FVector GetVelocity();
	// Set velocity to Vec
	void SetVelocity(FVector Vec);
	// Add Vec to current velocity
	void IncrementVelocity(FVector Vec);
	// Get current mass
	float GetMass();
	// Set mass to M
	void SetMass(float M);
	// Returns true if actor is active
	bool IsActive();
	// Get current type
	int GetType();
	// Set type to SphereType T
	void SetType(SphereType T);

	// Called when this actor collides with another
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	// Mesh and root component of the sphere
	UStaticMeshComponent *SphereMesh;

	// Point light component
	UPROPERTY(EditAnywhere)
	UPointLightComponent *Light;

	// Material/texture of the mesh (may be altered in the editor)
	UPROPERTY(EditAnywhere)
	class UMaterial* Material;

	// Array of all materials, loaded once
	TArray<UMaterial*> LoadedMaterials;

	// Is the actor active? False --> remove
	UPROPERTY(EditAnywhere)
	bool Active = true;

	// What kind of celestial body this sphere represents
	SphereType Type = Planet;

	// Current velocity of the sphere (can be altered in editor)
	UPROPERTY(EditAnywhere)
	FVector Velocity;

	// Current mass of the sphere (can be altered in editor)
	UPROPERTY(EditAnywhere)
	float Mass;

	// Current scale of the sphere (can be altered in editor)
	UPROPERTY(EditAnywhere)
	float Scale;

	// Creates a point light as the Light component
	void CreatePointLight();

};
