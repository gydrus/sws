// Simple World Simulator
// (с) GydruS 2023

#include "ASWS.h"
#include <iomanip>

TArray<TArray<FString>> ASWS::LoadBodiesFromCSVFileToArray(FString CompleteFilePath) {
	TArray<TArray<FString>> parsedCSV;

	if (FPaths::FileExists(CompleteFilePath))
	{
		FString FileContent;
		FFileHelper::LoadFileToString(FileContent, *CompleteFilePath);

		const TCHAR* Terminators[] = { L"\r", L"\n" }; //LINE_TERMINATOR
		const TCHAR* CSVDelimeters[] = { TEXT(",") };

		TArray<FString> CSVLines;
		FileContent.ParseIntoArray(CSVLines, Terminators, 2);

		TArray<FString> temp_array;
		for (int i = 0; i < CSVLines.Num(); i++) {
			temp_array.Empty();
			CSVLines[i].ParseIntoArray(temp_array, CSVDelimeters, 1);
			parsedCSV.Add(temp_array);
		}
	}

	/*for (int i = 0; i < parsedCSV.Num(); i++) {
		for (int j = 0; j < parsedCSV[i].Num(); j++) {
			UE_LOG(LogTemp, Error, TEXT("%s"), *parsedCSV[i][j]);
		}
		UE_LOG(LogTemp, Error, TEXT("%s"), "\n");
	}*/

	return parsedCSV;
}

bool ASWS::LoadBodies(USceneComponent* aRoot, FString CompleteFilePath, bool CallFromConstructor, bool SkipBoundParamsUpdate) {
	if (FPaths::FileExists(CompleteFilePath)) {
		TArray<TArray<FString>> StringsToLoad = LoadBodiesFromCSVFileToArray(CompleteFilePath);

		//UMaterial* BodiesMaterial = CreateDefaultSubobject<UMaterial>(TEXT("M_MateWhite"));

		if (StringsToLoad.Num() > 0) {
			WorldName = *StringsToLoad[0][0];

			if (!SkipBoundParamsUpdate) {
				if (StringsToLoad[1].Num() > 0) {
					double bm = FCString::Atof(*StringsToLoad[1][0]);	//Bound Mass
					boundMass = bm;
				}
				if (StringsToLoad[1].Num() > 1) {
					double bmd = FCString::Atof(*StringsToLoad[1][1]);	//Bound Mass Distance
					boundMassDistance = bmd;
				}
				if (StringsToLoad[1].Num() > 2) {
					double tVal = FCString::Atof(*StringsToLoad[1][2]);	//T Value
					t = tVal;
				}
				if (StringsToLoad[1].Num() > 3) {
					double mm = FCString::Atof(*StringsToLoad[1][3]);	//Mass Multiplier
					massMultiplier = mm;
				}
				if (StringsToLoad[1].Num() > 4) {
					int8 pm = FCString::Atoi(*StringsToLoad[1][4]);	//Phys mode
					PhysMode = (EPhysModes)pm;
				}
			}

			for (int i = 2; i < StringsToLoad.Num(); i++) {
				if (StringsToLoad[i].Num() < 2) continue;

				FString name = "Body #" + FString::FromInt(i-1);
				UTheBody* Body;
				if (CallFromConstructor) {
					Body = CreateDefaultSubobject<UTheBody>(*name);
					Body->SetupAttachment(aRoot);
				}
				else {
					Body = NewObject<UTheBody>(this);
					Body->RegisterComponent();
					Body->AttachToComponent(aRoot, FAttachmentTransformRules::KeepRelativeTransform);
				}

				float x = FCString::Atof(*StringsToLoad[i][0]);
				float y = FCString::Atof(*StringsToLoad[i][1]);
				float z = FCString::Atof(*StringsToLoad[i][2]);
				float m = 1;
				if (StringsToLoad[i].Num() > 3) m = FCString::Atof(*StringsToLoad[i][3]);	//Mass
				float s = m;
				if (StringsToLoad[i].Num() > 4) s = FCString::Atof(*StringsToLoad[i][4]);	//Size
				float mi = 0;
				if (StringsToLoad[i].Num() > 5) mi = FCString::Atoi(*StringsToLoad[i][5]);	//Material Index
				if (StringsToLoad[i].Num() > 6) Body->BodyName = *StringsToLoad[i][6];		//Name
				float ivx = 0, ivy = 0, ivz = 0;
				if (StringsToLoad[i].Num() > 7) ivx = FCString::Atof(*StringsToLoad[i][7]);	//Initial vector X
				if (StringsToLoad[i].Num() > 8) ivy = FCString::Atof(*StringsToLoad[i][8]);	//Initial vector Y
				if (StringsToLoad[i].Num() > 9) ivz = FCString::Atof(*StringsToLoad[i][9]);	//Initial vector Z


				Body->m = m * massMultiplier;
				Body->s = s;
				Body->mi = mi;
				Body->v = FVector(ivx, ivy, ivz);

				Body->index = Bodies.Add(Body);

				if (SphereMeshAsset != nullptr) {
					Body->SetStaticMesh(SphereMeshAsset);
					SetMeshSize(Body, s);

					//Body->SetRelativeLocation(Body->GetRelativeLocation() + FVector(x, y, z));
					Body->SetWorldLocation(FVector(x, y, z));
					if (mi < BodiesMaterials.Num()) {
						if (BodiesMaterials[mi]) Body->SetMaterial(0, BodiesMaterials[mi]);
					}
				}

			}
			return true;
		}
	}

	return false;
}

void ASWS::LogBodies() {
	for (int16 i = 0; i < Bodies.Num(); i++) {
		UTheBody* Body = Bodies[i];
		UE_LOG(LogTemp, Warning, TEXT("Body: %s"), *Body->GetName());
	}
}

void ASWS::SetMeshSize(UStaticMeshComponent* Mesh, float Size) {
	FTransform transf;
//	FString str = Mesh->CalcBounds(transf).GetBox().GetSize().ToString();
//	UE_LOG(LogTemp, Warning, TEXT("MeshSizeOriginal: %s"), *str);
	//Mesh->CalcBounds().GetBox().GetSize().ToString();
//	UE_LOG(LogTemp, Warning, TEXT("Size to set: %f"), Size);
	//Mesh->SetWorldScale3D(FVector(Size, Size, Size));
	Mesh->SetRelativeScale3D(FVector(Size, Size, Size));
//	str = Mesh->CalcBounds(transf).BoxExtent.ToString();
//	UE_LOG(LogTemp, Warning, TEXT("MeshSizeResult: %s"), *str);
}


void ASWS::Init(bool CallFromConstructor, bool SkipBoundParamsUpdate) {
	// Создаем компонент для отображения глобальный сферы приталкивания
	if (CallFromConstructor) {
		GlobalSphere = CreateDefaultSubobject<UStaticMeshComponent>("Global Sphere");
		GlobalSphere->SetupAttachment(Root);
	}
	else {
		GlobalSphere = NewObject<UStaticMeshComponent>(this);
		GlobalSphere->RegisterComponent();
		GlobalSphere->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	}

	if (SphereMeshAsset != nullptr) {
		GlobalSphere->SetStaticMesh(SphereMeshAsset);
		//GlobalSphere->SetRelativeLocation(FVector(0, 0, 0));

		//UMaterial* GlobalSphereMaterial = CreateDefaultSubobject<UMaterial>(TEXT("/Materials/M_ForceSphere"));
		//auto GlobalSphereMaterial = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'Materials/M_ForceSphere.uasset'")); //CreateDefaultSubobject<UMaterial>(TEXT("C:\\Users\\GydruS\\Documents\\Unreal Projects\\KatWorld_alpha_03\\Content\\Materials\\M_ForceSphere.uasset"));
		//ConstructorHelpers::FObjectFinder MaterialAsset(TEXT(“Material / MATERIAL PATH’”));
		if (GlobalSphereMaterial) GlobalSphere->SetMaterial(0, GlobalSphereMaterial);

		GlobalSphere->SetWorldLocation(FVector(0, 0, 0));
		SetMeshSize(GlobalSphere, boundMassDistance);
	}

	// Загружаем конфигурацию тел из файла
	bool result = LoadBodies(Root, FileName, CallFromConstructor, SkipBoundParamsUpdate);
	GlobalSphere->SetVisibility(PhysMode == EPhysModes::Katuschik);

	//LogBodies();
}

void ASWS::Clear() {
	for (int16 i = 0; i < Bodies.Num(); i++) {
		UTheBody* Body = Bodies[i];
		if (Body) {
			Body->DetachFromParent();
			Body->DestroyComponent();
		}
	}
	GlobalSphere->DetachFromParent();
	GlobalSphere->DestroyComponent();
	Bodies.Empty();
}

void ASWS::Restart(bool SkipBoundParamsUpdate) {
	Clear();
	Init(false, SkipBoundParamsUpdate);
}

void ASWS::Stop() {
	pause = true;
}

void ASWS::Step() {
	BodyMove();
}

void ASWS::Play() {
	pause = false;
}

void ASWS::LoadFromFile() {
	FString defaultFile = FileName;
	FString Path = FPaths::GetPath(FileName);
	FString fileTypes = "*.txt";
	TArray<FString> outFileNames;
	uint32 flags = 1;
	IDesktopPlatform* fpl = FDesktopPlatformModule::Get();

	fpl->OpenFileDialog(0, "dialogName", Path, defaultFile, fileTypes, flags, outFileNames);

	for (int16 i = 0; i < outFileNames.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("file: %s"), *outFileNames[i]);
		FileName = *outFileNames[i];
		Restart();
		break;
	}
}


// Sets default values
ASWS::ASWS()
{
	auto new_val = std::setprecision(13);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Создаем корневой компонент
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (MeshAsset.Object != nullptr) {
		SphereMeshAsset = MeshAsset.Object;
	}

	FString dir = FPaths::ProjectDir();
	FileName = dir + "bodies.txt";

	//Clear();
	Init(true);
}

void ASWS::BodyMove() {
	double G = 6.67408e-11;	// гравитационная постоянная		// G = 6.67408e-5; (4 test)
	//UE_LOG(LogTemp, Warning, TEXT("      G=%lf"), G);

	TArray<FVector> forces;	// сюда будем сохранять силы, которые действуют в моменте на тела

	// вычисляем силы, которые действуют в моменте на тела
	for (int16 i = 0; i < Bodies.Num(); i++) {
		UTheBody* Body = Bodies[i];
		FVector resultForceVector = FVector(0, 0, 0);
		double M = Body->m;

		for (int16 j = 0; j < Bodies.Num(); j++) {
			if (i == j) continue;
			UTheBody* Body2 = Bodies[j];
			FVector forceVectorDirection = FVector(0, 0, 0);
			double distance = 0; // = (Body->GetRelativeLocation() - Body2->GetRelativeLocation()).Size();
			(Body->GetRelativeLocation() - Body2->GetRelativeLocation()).ToDirectionAndLength(forceVectorDirection, distance);
			double m = Body2->m;
			double F = 0;// G* M* m / (distance * distance); //F = G*(Mm/r2);
			
			if (distance != 0) {
				switch (PhysMode) {
					case EPhysModes::Katuschik: F = G * M * m / (distance * distance); break;	//F = G*(Mm/r2);
					case EPhysModes::Newton: F = -G * M * m / (distance * distance); break;		//F = -G*(Mm/r2);
				}
			}

			FVector forceVector = forceVectorDirection * F;
			resultForceVector += forceVector;
			if (/*i == 4 && */outputLog) {
				//UE_LOG(LogTemp, Warning, TEXT("forceVector[%s-%s]: %s, F:%f"), *Body->GetName(), *Body2->GetName(), *forceVector.ToString(), F);
			}
		}

		if (/*i == 4 && */outputLog) {
			UE_LOG(LogTemp, Warning, TEXT("      forceVector[%s]: %s"), *Body->GetName(), *resultForceVector.ToString());
		}

		if (PhysMode == EPhysModes::Katuschik) {
			// вычисляем воздействие сферы приталкивания
			FVector boundForceVectorDirection = FVector(0, 0, 0); double distanceToCenter = 0; double distanceToBound = 0;
			//		(FVector::Zero() - Body->GetRelativeLocation()).ToDirectionAndLength(boundForceVectorDirection, distance); // Воздействие силы от сферы приталкивания направляем в центр отсчета в нашей системе
			(FVector::Zero() - Body->GetComponentLocation()).ToDirectionAndLength(boundForceVectorDirection, distanceToCenter); // Воздействие силы от сферы приталкивания направляем в центр отсчета в нашей системе, distanceToCenter - расстояние от текущего тела до центра системы
			distanceToBound = boundMassDistance - abs(distanceToCenter); // получаем расстояние от границы сферы приталкивания до текущего тела
			if (/*i == 4 && */outputLog) UE_LOG(LogTemp, Warning, TEXT("BoundForceVector: %s, distance to center: %f, distance to bound: %f"), *boundForceVectorDirection.ToString(), distanceToCenter, distanceToBound);

			if (distanceToBound > 0) { // тело находится ВНУТРИ сферы приталкивания
				double F = G * M * (boundMass * massMultiplier) / (distanceToBound * distanceToBound); //F = G*(Mm/r2);
				FVector boundForceVector = boundForceVectorDirection * F;
				resultForceVector += boundForceVector;
				//if (i == 4 && outputLog) {
				if (outputLog) {
					UE_LOG(LogTemp, Warning, TEXT(" boundForceVector[%s]: %s, F: %f"), *Body->GetName(), *boundForceVector.ToString(), F);
					UE_LOG(LogTemp, Warning, TEXT("resultForceVector[%s]: %s"), *Body->GetName(), *resultForceVector.ToString());
				}
			}
		}

		forces.Add(resultForceVector);
	}

	if (outputLog) UE_LOG(LogTemp, Warning, TEXT("---"));

	// перемещаем тела согласно рассчитанным силам
	for (int16 i = 0; i < Bodies.Num(); i++) {
		UTheBody* Body = Bodies[i];
		FVector a = forces[i] / Body->m;
		if (!BackWay) {
			Body->v += a * t;
			Body->SetRelativeLocation(Body->GetRelativeLocation() + Body->v * t);
		}
		else {
			Body->v -= a * t;
			Body->SetRelativeLocation(Body->GetRelativeLocation() - Body->v * t);
		}
	}
}

// Called when the game starts or when spawned
void ASWS::BeginPlay()
{
	Super::BeginPlay();
	Restart();
}

// Called every frame
void ASWS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!pause) BodyMove();
}

void ASWS::GetTValue(float& tVal) { tVal = t; }
void ASWS::SetTValue(float tVal) { t = tVal; }
void ASWS::GetBoundMassValue(float& Val) { Val = boundMass; }
void ASWS::SetBoundMassValue(float Val) { boundMass = Val; }
void ASWS::GetBoundMassDistanceValue(float& Val) { Val = boundMassDistance; }
void ASWS::SetBoundMassDistanceValue(float Val) {
	boundMassDistance = Val; 
	SetMeshSize(GlobalSphere, boundMassDistance);
}
void ASWS::GetMassMultiplier(float& Val) { Val = massMultiplier; }
void ASWS::GetWorldName(FString& name) { name = WorldName; };
void ASWS::SetBackWaySimulation(bool BackWaySimulation) {
	BackWay = BackWaySimulation;
}


void ASWS::SetBodySelected(int bodyIndex, bool selected) {
	if (bodyIndex > -1 && bodyIndex < Bodies.Num()) {
		UTheBody* Body = Bodies[bodyIndex];
		Body->selected = selected;
		if (Body->selected) {
			Body->SetOverlayMaterial(BodySelectedMaterial);
		}
		else {
			Body->SetOverlayMaterial(nullptr);
		}
	}
}

void ASWS::DeselectBodies() {
	for (int16 i = 0; i < Bodies.Num(); i++) {
		UTheBody* Body = Bodies[i];
		Body->SetOverlayMaterial(nullptr);
	}
}

void ASWS::SetPhysMode(EPhysModes mode) {
	PhysMode = mode;
	GlobalSphere->SetVisibility(mode == EPhysModes::Katuschik);
	if (mode == EPhysModes::Katuschik) { UE_LOG(LogTemp, Log, TEXT("Katuschik")); }
	else { UE_LOG(LogTemp, Log, TEXT("Newton")); };
}

EPhysModes ASWS::GetPhysMode() {
	return PhysMode;
}

