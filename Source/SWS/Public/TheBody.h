// Simple World Simulator
// (c) GydruS 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "TheBody.generated.h"

/**
 * 
 */
UCLASS()
class SWS_API UTheBody : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString BodyName = "The Body";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PhysParams) double m;		// Mass
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PhysParams) double s;		// Size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderParams) int mi = 0;	// Material Index
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderParams) bool selected = false;	// Selected or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PhysParams) FVector v;		// Vector
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PhysParams) FVector iv;		// Initial vector
	UPROPERTY(BlueprintReadOnly, Category = RenderParams) int index = -1;				// Index in world items array
};
