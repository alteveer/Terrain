#include "Chunk.h"

void UChunk::Init(FIntVector loc, int32 chunk_size, int32 block_size)
{
	m_Location = loc;
	m_ChunkSize = chunk_size;
	m_BlockSize = block_size;
	m_Corners.Init(CORNER({ 0.0 }), chunk_size * chunk_size * chunk_size);
}

const CORNER & UChunk::get_corner(int32 xx, int32 yy, int32 zz)
{
	int32 idx = xx + (m_ChunkSize * yy) + (m_ChunkSize * m_ChunkSize * zz);
	return m_Corners[idx];
}

const CORNER & UChunk::get_corner(FIntVector v)
{
	return get_corner(v.X, v.Y, v.Z);
}

void UChunk::set_corner(int32 xx, int32 yy, int32 zz, double val)
{
	int32 key = xx + (m_ChunkSize * yy) + (m_ChunkSize * m_ChunkSize * zz);
	m_Corners[key].val = val;
}

const FIntVector UChunk::location()
{
	return m_Location;
}

const FVector UChunk::world_location()
{
	return FVector(m_Location) * FVector(m_ChunkSize) * FVector(m_BlockSize);
}

const uint8 UChunk::chunksize()
{
	return m_ChunkSize;
}
