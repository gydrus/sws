// Simple World Simulator
// (с) GydruS 2023

#pragma once

#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
//#include "FileHelpers.h"
#include "Engine/StaticMesh.h"
#include "String.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "TheBody.h"
//#include "Misc/Paths.h"
#include "ASWS.generated.h"

struct BodyConfig {
	double m;		// Mass
	double s;		// Size
	int16 mi = 0;	// Material Index
	FVector v;		// Initial vector
};

UENUM(BlueprintType)
enum class EPhysModes : uint8 { Newton = 0, Katuschik = 1 };

UCLASS()

class SWS_API ASWS : public AActor
{
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	ASWS();

	UPROPERTY(VisibleAnywhere) USceneComponent* Root;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* GlobalSphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials) UMaterial* GlobalSphereMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials) TArray<UMaterial*> BodiesMaterials;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials) UMaterial* BodySelectedMaterial;
	UPROPERTY(EditAnywhere) double boundMassDistance = 200;
	UPROPERTY(EditAnywhere) double boundMass = 10;

	UPROPERTY(EditAnywhere) double massMultiplier = 1000000;	// дл€ отображени€ (в csv-фалйе и не только) значений в тыс€чах/млн/млрд и т.д. тонн
	UPROPERTY(EditAnywhere) double t = 1;						// физическое врем€, проход€щее за один цикл расчетов (вроде точности моделировани€: 1 - точнее, 10 - быстрее)
	UPROPERTY(EditAnywhere) bool outputLog = true;
	UPROPERTY(EditAnywhere) bool pause = false;
	UPROPERTY(EditAnywhere) bool BackWay = false;
	UPROPERTY(EditAnywhere) FString FileName = ".\\bodies.txt";
	UPROPERTY(EditAnywhere) FString WorldName = "Incredible World!";
	UPROPERTY(VisibleAnywhere) TArray<UTheBody*> Bodies;
	UPROPERTY(EditAnywhere) EPhysModes PhysMode = EPhysModes::Katuschik;


	TArray<TArray<FString>> LoadBodiesFromCSVFileToArray(FString CompleteFilePath = ".\\bodies.txt");
	bool LoadBodies(USceneComponent* aRoot, FString CompleteFilePath = ".\\bodies.txt", bool CallFromConstructor = false, bool SkipBoundParamsUpdate = false);
	void LogBodies();
	void BodyMove();
	void SetMeshSize(UStaticMeshComponent* Mesh, float Size);
	void Init(bool CallFromConstructor = false, bool SkipBoundParamsUpdate = false);
	void Clear();

	UFUNCTION(BlueprintCallable) void Restart(bool SkipBoundParamsUpdate = false);
	UFUNCTION(BlueprintCallable) void Stop();
	UFUNCTION(BlueprintCallable) void Step();
	UFUNCTION(BlueprintCallable) void Play();
	UFUNCTION(BlueprintCallable) void LoadFromFile();
	UFUNCTION(BlueprintCallable) void GetTValue(float& tVal);
	UFUNCTION(BlueprintCallable) void SetTValue(float tVal);
	UFUNCTION(BlueprintCallable) void SetBackWaySimulation(bool BackWaySimulation);
	UFUNCTION(BlueprintCallable) void GetBoundMassValue(float& tVal);
	UFUNCTION(BlueprintCallable) void SetBoundMassValue(float tVal);
	UFUNCTION(BlueprintCallable) void GetBoundMassDistanceValue(float& tVal);
	UFUNCTION(BlueprintCallable) void SetBoundMassDistanceValue(float tVal);
	UFUNCTION(BlueprintCallable) void GetMassMultiplier(float& tVal);
	UFUNCTION(BlueprintCallable) void GetWorldName(FString& name);
	UFUNCTION(BlueprintCallable) void SetBodySelected(int bodyIndex = -1, bool selected = false);
	UFUNCTION(BlueprintCallable) void DeselectBodies();
	UFUNCTION(BlueprintCallable) void SetPhysMode(EPhysModes mode = EPhysModes::Katuschik);
	UFUNCTION(BlueprintCallable) EPhysModes GetPhysMode();


protected:
	UStaticMesh* SphereMeshAsset;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
