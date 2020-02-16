// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"
#include "ChunkManagerComponent.h"

//#include "FastNoise/FastNoise.h"

#include "TerrainActor.generated.h"

class URuntimeMeshComponent;
class UChunkManagerComponent;

UENUM()
enum class ECubeside : uint8
{ 
	BOTTOM, TOP, LEFT, RIGHT, FRONT, BACK,
};

USTRUCT()
struct FTerrainVertexData
{
	GENERATED_BODY()
public: 
	UPROPERTY()
	TArray<FVector> Vertices;
	UPROPERTY()
	TArray<FVector> Normals;
	UPROPERTY()
	TArray<FVector2D> TextureCoordinates;
	UPROPERTY()
	TArray<int32> Triangles;
	
	UPROPERTY()
	TArray<FRuntimeMeshTangent> Tangents;
	UPROPERTY()
	TArray<FColor> VertexColors;
};

UCLASS()
class TERRAIN_API ATerrainActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATerrainActor();


protected:
	void UpdateChunkStreaming();
	void DrawChunk(UChunk & chunk, int32 section);
	void MarchCube(GRIDCELL cell, FTerrainVertexData & vdata);
	void CreateQuad(ECubeside side, FVector pos, FTerrainVertexData& vdata);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URuntimeMeshComponent* m_TerrainMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UChunkManagerComponent* m_ChunkManagerComponent;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	//	uint8 WorldSizeX = 16;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	//	uint8 WorldSizeY = 16;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		UMaterialInterface* DefaultMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		FVector CameraLocation = FVector(0, 0, 0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		uint8 CameraChunkRadius = 4;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		uint8 ChunkSize = 16;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		int32 BlockSize = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		int32 WorldSizeX = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		int32 WorldSizeY = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		int32 WorldSizeZ = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float WorldFloor = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float WorldCeil = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float IsoLevel = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float Frequency = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float Lacunarity = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float Amplitude = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float Persistence = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float HeightFalloff = 400.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float Angle = 90.0;
	UPROPERTY()
		int32 mesh_section = 0;
	TMap<FIntVector, int32> m_MeshSectionLookup;
//	FastNoise myNoise; // Create a FastNoise object

public:
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// all possible UVs
	static const FVector2D uv00;
	static const FVector2D uv10;
	static const FVector2D uv01;
	static const FVector2D uv11;

	// all possible vertices
	static const FVector p0;
	static const FVector p1;
	static const FVector p2;
	static const FVector p3;
	static const FVector p4;
	static const FVector p5;
	static const FVector p6;
	static const FVector p7;

	
};

