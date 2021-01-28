// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkManagerComponent.h"
#include "srdnoise23.h"



// Sets default values for this component's properties
UChunkManagerComponent::UChunkManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UChunkManagerComponent::Init(
	uint8 chunk_size, uint16 block_size, FIntVector world_size,
	float world_floor, float world_ceil, float height_falloff, float angle,
	float amplitude, float persistence, float frequency, float lacunarity
) {
	m_ChunkSize = chunk_size;
	m_BlockSize = block_size;
	m_WorldSize = world_size;
	m_WorldFloor = world_floor;
	m_WorldCeil = world_ceil;
	
	// noise params
	HeightFalloff = height_falloff;

	Amplitude = amplitude;
	Persistence = persistence;
	Frequency = frequency / 100000.0f;
	Lacunarity = lacunarity;
	Angle = angle;

	m_Chunks.Empty();

}
TArray<UChunk*> UChunkManagerComponent::GetChunksAroundCamera(FVector cam_location, uint8 camera_chunk_radius)
{
	TArray<UChunk*> out_chunks;
	FIntVector cam_chunk_vector = WorldToChunkLocation(cam_location);
	for (int z = cam_chunk_vector.Z - camera_chunk_radius; z <= cam_chunk_vector.Z + camera_chunk_radius; z++)
	{
		for (int y = cam_chunk_vector.Y - camera_chunk_radius; y <= cam_chunk_vector.Y + camera_chunk_radius; y++)
		{
			for (int x = cam_chunk_vector.X - camera_chunk_radius; x <= cam_chunk_vector.X + camera_chunk_radius; x++)
			{
				FIntVector key = FIntVector(x, y, z);
				auto ch = GetOrCreateChunk(key);
				if (ch == nullptr)
					continue;
				out_chunks.Add(ch);
			}
		}
	}
	return out_chunks;
}

FIntVector UChunkManagerComponent::WorldToChunkLocation(FVector loc)
{
	FIntVector chunk_key = FIntVector(loc / float(m_BlockSize * m_ChunkSize));
	return chunk_key;
}
UChunk * UChunkManagerComponent::GetOrCreateChunk(uint16 xx, uint16 yy, uint16 zz)
{
	return GetOrCreateChunk(FIntVector( xx, yy, zz ));
}
UChunk * UChunkManagerComponent::GetOrCreateChunk(FIntVector location)
{
	
	if (FMath::Abs(location.X) > m_WorldSize.X - 1 ||
		FMath::Abs(location.Y) > m_WorldSize.Y - 1 ||
		FMath::Abs(location.Z) > m_WorldSize.Z - 1)
	{
		return nullptr;
	}
	if (!m_Chunks.Contains(location))
		CreateChunk(location);

	return m_Chunks[location];
}


// Called when the game starts
void UChunkManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UChunkManagerComponent::CreateChunk(FIntVector location)
{
	auto new_chunk = NewObject<UChunk>();
	new_chunk->Init(location, m_ChunkSize, m_BlockSize);
	auto world_loc = new_chunk->world_location();
	for (int z = 0; z < m_ChunkSize; z++)
	{
		for (int y = 0; y < m_ChunkSize; y++)
		{
			for (int x = 0; x < m_ChunkSize; x++)
			{
				//FVector pos = world_loc + FVector(x, y, z); * FVector(m_BlockSize);
				FVector pos = world_loc + FVector(x, y, z) * FVector(m_BlockSize);
				
				double noise_val = 0.0;
				for (int octave = 0; octave < 6; octave++) {
					noise_val += srdnoise2(pos.X, pos.Y, Frequency * FMath::Pow(Lacunarity, octave), Angle, nullptr, nullptr) * (Amplitude * FMath::Pow(Persistence, octave));
				}

				noise_val = ((noise_val * HeightFalloff + HeightFalloff / 1.5) - pos.Z) / HeightFalloff;
				//float noise2 =	srdnoise2(pos.X, pos.Y, Frequency, Angle, nullptr, nullptr) * Om0 +
				//				srdnoise2(pos.X, pos.Y, Frequency * Fm1, Angle + Am1, nullptr, nullptr) * Om1 +
				//				srdnoise2(pos.X, pos.Y, Frequency * Fm2, Angle - Am2, nullptr, nullptr) * Om2;
				//noise2 /= Om0 + Om1 + Om2;
				//double noise_val = ((noise2 * HeightFalloff + HeightFalloff / 1.5) - pos.Z) / HeightFalloff;

				//double noise_val = srdnoise3(pos.X, pos.Y, pos.Z, Frbequency / 10000.0f, Angle, nullptr, nullptr, nullptr) / 2.0f + 0.5f;
				//double noise_val = FMath::Sin(pos.X) + FMath::Sin(pos.Y) + FMath::Sin(pos.Z);
				//noise_val /= 3.0f;
				//noise_val = noise_val / 2.0f + 0.5f;
				//double noise_val = float(y) / float(m_ChunkSize);
				new_chunk->set_corner(x, y, z, noise_val);
			}
		}
	}

	m_Chunks.Add(location, new_chunk);
	//m_Chunks[location] = new_chunk;
}

const GRIDCELL UChunkManagerComponent::GetGridcell(FIntVector chunk_xyz, FIntVector gridcell_xyz)
{
	uint8 xx = gridcell_xyz.X;
	uint8 yy = gridcell_xyz.Y;
	uint8 zz = gridcell_xyz.Z;
	
	FIntVector corner_positions[8] = {
		{ xx,		yy,		zz },
		{ xx + 1,	yy,		zz },
		{ xx + 1,	yy + 1,	zz },
		{ xx,		yy + 1,	zz },
		{ xx,		yy,		zz + 1 },
		{ xx + 1,	yy,		zz + 1 },
		{ xx + 1,	yy + 1,	zz + 1 },
		{ xx,		yy + 1,	zz + 1 }
	};
	

	GRIDCELL gridcell = GRIDCELL();

	for (int p = 0; p < 8; p++)
	{
		FIntVector chunk_index = chunk_xyz;
		FIntVector corner_index = corner_positions[p];

		//check for outside of chunk bounds
		if (corner_positions[p].X >= m_ChunkSize) {
			chunk_index.X += 1;
			corner_index.X = 0;
		}
		if (corner_positions[p].Y >= m_ChunkSize) {
			chunk_index.Y += 1;
			corner_index.Y = 0;
		}
		if (corner_positions[p].Z >= m_ChunkSize) {
			chunk_index.Z += 1;
			corner_index.Z = 0;
		}

		gridcell.p[p] = ((FVector(chunk_index) * m_ChunkSize) + FVector(corner_index)) * FVector(m_BlockSize);

		auto chunk = GetOrCreateChunk(chunk_index);
		//check for outside of world bounds/no chunk
		if (chunk == nullptr || gridcell.p[p].Z > m_WorldCeil)
		{
			gridcell.val[p] = 0.0f;
		}
		else if (gridcell.p[p].Z < m_WorldFloor)
		{
			gridcell.val[p] = 1.0f;
		}
		else 
		{
			auto corner = chunk->get_corner(corner_index);
			gridcell.val[p] = corner.val;
		}

		//UE_LOG(LogTemp, Warning, TEXT("in grid: %s\t chunk: %s\t cell: %s \t%02f\t %s /t - %s"), 
		//	*gridcell_xyz.ToString(), 
		//	*chunk_index.ToString(), 
		//	*corner_index.ToString(), 
		//	gridcell.val[p], 
		//	*tmp.ToString(),
		//	*gridcell.p[p].ToString());
	}
	

	return gridcell;
}

// Called every frame
void UChunkManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

