// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Chunk.h"
#include "MarchingCubes.h"

#include "ChunkManagerComponent.generated.h"


UCLASS( meta=(BlueprintSpawnableComponent) )
class TERRAIN_API UChunkManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	//const uint16 WORLD_OFFSET = 16384;
	
	UChunkManagerComponent(const FObjectInitializer& ObjectInitializer);
	//TODO: maybe do world bounds instead you idiot
	void Init(
		uint8 chunk_size,
		uint16 block_size,
		FIntVector world_size,
		float world_floor, float world_ceil, float height_falloff, float angle,
		float amplitude, float persistence, float frequency, float lacunarity
		);

	TArray<UChunk*> GetChunksAroundCamera(FVector cam_location, uint8 camera_chunk_radius);
	
	UChunk* GetOrCreateChunk(FIntVector location);
	UChunk* GetOrCreateChunk(uint16 xx, uint16 yy, uint16 zz);

	FIntVector WorldToChunkLocation(FVector loc);
	const GRIDCELL GetGridcell(FIntVector chunk_xyz, FIntVector gridcell_xyz);

	float HeightFalloff = 400.0;
	float Angle = 90.0;

	float Amplitude = 200.0;
	float Persistence = 0.5;
	float Frequency = 0.001;
	float Lacunarity = 2.0;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void CreateChunk(FIntVector location);
	uint8 m_ChunkSize;
	uint16 m_BlockSize;
	FIntVector m_WorldSize;
	float m_WorldFloor;
	float m_WorldCeil;

	UPROPERTY()
	TMap<FIntVector, UChunk*> m_Chunks;

private:

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
