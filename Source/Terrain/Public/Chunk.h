// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Chunk.generated.h"


typedef struct {
	double val;
} CORNER;

UCLASS()
class TERRAIN_API UChunk : public UObject
{
	GENERATED_BODY()
public:
	void Init(FIntVector loc, int32 chunk_size, int32 block_size);

	const CORNER & get_corner(int32 xx, int32 yy, int32 zz);
	const CORNER & get_corner(FIntVector v);
	void set_corner(int32 xx, int32 yy, int32 zz, double val);

	const FIntVector location();
	const FVector world_location();
	const uint8 chunksize();

protected:
	FIntVector m_Location;
	uint8 m_ChunkSize;
	uint16 m_BlockSize;
	
	TArray<CORNER> m_Corners;
	
};