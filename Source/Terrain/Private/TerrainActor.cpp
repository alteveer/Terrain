// Fill out your copyright notice in the Description page of Project Settings.

#include "TerrainActor.h"
#include "Engine/CollisionProfile.h"

const FVector2D ATerrainActor::uv00 = FVector2D(0, 0);
const FVector2D ATerrainActor::uv10 = FVector2D(1, 0);
const FVector2D ATerrainActor::uv01 = FVector2D(0, 1);
const FVector2D ATerrainActor::uv11 = FVector2D(1, 1);

// all possible vertices
const FVector ATerrainActor::p0 = FVector(-50, 50, -50);
const FVector ATerrainActor::p1 = FVector(50, 50, -50);
const FVector ATerrainActor::p2 = FVector(50, -50, -50);
const FVector ATerrainActor::p3 = FVector(-50, -50, -50);
const FVector ATerrainActor::p4 = FVector(-50, 50, 50);
const FVector ATerrainActor::p5 = FVector(50, 50, 50);
const FVector ATerrainActor::p6 = FVector(50, -50, 50);
const FVector ATerrainActor::p7 = FVector(-50, -50, 50);

// Sets default values
ATerrainActor::ATerrainActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_TerrainMeshComponent = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("Terrain0"));
	m_TerrainMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	m_TerrainMeshComponent->Mobility = EComponentMobility::Static;
	m_TerrainMeshComponent->SetMeshSectionCollisionEnabled(0, true);

#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 20
	m_TerrainMeshComponent->SetGenerateOverlapEvents(false);
#else
	m_TerrainMeshComponent->bGenerateOverlapEvents = false;
#endif
	m_TerrainMeshComponent->ClearAllMeshSections();

	m_ChunkManagerComponent = CreateDefaultSubobject<UChunkManagerComponent>(TEXT("ChunkMan0"));
	RootComponent = m_TerrainMeshComponent;
	//myNoise.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	
}
void ATerrainActor::OnConstruction(const FTransform& Transform)
{
	m_ChunkManagerComponent->Init(
		ChunkSize, BlockSize, FIntVector({ WorldSizeX, WorldSizeY, WorldSizeZ }),
		WorldFloor, WorldCeil, HeightFalloff, Angle,
		Amplitude, Persistence, Frequency, Lacunarity
	);
	//myNoise.SetFrequency(Frequency);
	UpdateChunkStreaming();
}
void ATerrainActor::UpdateChunkStreaming()
{
	auto chunks = m_ChunkManagerComponent->GetChunksAroundCamera(CameraLocation, CameraChunkRadius);
	TArray<FIntVector> resident_chunks;
	
	for (const auto &chunk : chunks)
	{
		FIntVector loc = (*chunk).location();
		if (!m_MeshSectionLookup.Contains(loc))
		{
			m_MeshSectionLookup.Add(loc, mesh_section);
			DrawChunk(*chunk, mesh_section);
			mesh_section++;
		}
		resident_chunks.Add(loc);
	}

	for (auto it = m_MeshSectionLookup.CreateIterator(); it; ++it)
	{
		if(!resident_chunks.Contains(it->Key))
		{
			m_TerrainMeshComponent->ClearMeshSection(it->Value);
			it.RemoveCurrent();
		}
	}
	
}
void ATerrainActor::DrawChunk(UChunk & chunk, int32 section)
{
	FTerrainVertexData vertex_data = FTerrainVertexData();
	//FVector chunk_offset = FVector(
	//	chunk.location().X * BlockSize * chunk.chunksize(),
	//	chunk.location().Y * BlockSize * chunk.chunksize(),
	//	chunk.location().Z * BlockSize * chunk.chunksize()
	//);
	//UE_LOG(LogTemp, Warning, TEXT("chunk: %03d - %s\t** %s"),
		//mesh_section, *chunk.location().ToString(), *chunk.world_location().ToString());

	for (int z = 0; z < chunk.chunksize(); z++)
	{
		for (int y = 0; y < chunk.chunksize(); y++)
		{
			for (int x = 0; x < chunk.chunksize(); x++)
			{
				GRIDCELL cell = m_ChunkManagerComponent->GetGridcell(chunk.location(), FIntVector(x, y, z));
				MarchCube(cell, vertex_data);
				
					
				//CreateQuad(ECubeside::TOP, FVector(x, y, z), vertex_data);
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("chunk: %03d - %s\t%s \tvertex_count: %06d"),
	//	mesh_section, *chunk.world_location().ToString(), *chunk.location().ToString(), vertex_data.Vertices.Num());

	//uint8 section_index = chunk_pos_y * WorldSize + chunk_pos_x;
	m_TerrainMeshComponent->ClearMeshSection(section);
	m_TerrainMeshComponent->GetOrCreateRuntimeMesh()->CreateMeshSection(
		section, 
		vertex_data.Vertices, vertex_data.Triangles, vertex_data.Normals, 
		vertex_data.TextureCoordinates, vertex_data.VertexColors, vertex_data.Tangents, true);

	m_TerrainMeshComponent->GetOrCreateRuntimeMesh()->SetSectionMaterial(section, DefaultMaterial);
}
void ATerrainActor::MarchCube(GRIDCELL cell, FTerrainVertexData & vdata)
{
	TRIANGLE triangles[16];


	int ntri = Polygonise(cell, IsoLevel, triangles);
	for (int t = 0; t < ntri; t++)
	{
		auto t_o = vdata.Vertices.Num();
		vdata.Vertices.Append(
			{ triangles[t].p[0], triangles[t].p[1], triangles[t].p[2] }
		);
		FVector face_normal = -FVector::CrossProduct(triangles[t].p[0] - triangles[t].p[1], triangles[t].p[0] - triangles[t].p[2]);
		face_normal.Normalize();
		vdata.Normals.Append(
			{ face_normal, face_normal, face_normal }
		);
		
		
		FVector color_vec = FVector();
		color_vec.X = FVector::DotProduct(face_normal, FVector(0.0f, 0.0f, 1.0f));
		//color_vec *= FVector(255.0f);
		FColor vcol = FColor(color_vec.X, color_vec.Y, color_vec.Z);
		vdata.VertexColors.Append({ vcol, vcol, vcol });
		vdata.Triangles.Append({ t_o, t_o + 1, t_o + 2 });
	}
	
}

// Called when the game starts or when spawned
void ATerrainActor::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ATerrainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

