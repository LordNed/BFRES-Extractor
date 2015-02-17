#pragma once

#include <stdint.h>

// It sounds like GTX and GFD can be used interchangably and they share the same header/block header
struct GTXHeader
{
	char Magic[4]; // GFX2
	uint32_t HeaderSize; // Always 32
	uint32_t MajorVersion;
	uint32_t MinorVersion;
	uint32_t GpuVersion;
	uint32_t AlignmentMode;
	uint32_t Reserved1;
	uint32_t Reserved2;
};

struct GTXBlockHeader
{
	char Magic[4]; // BLK{
	uint32_t HeaderSize; // Always 32
	uint32_t MajorVersion;
	uint32_t MinorVersion;
	uint32_t Type;
	uint32_t DataSize;
	uint32_t Id;
	uint32_t TypeIndex;
};

struct GTXBlockRelocationHeader
{
	char Magic[4]; // ?
	uint32_t Size;
	uint32_t Type;
	uint32_t DataSize;
	uint32_t DataOffset;
	uint32_t StringTableCharNumber; // String Table index?
	uint32_t StringTableOffset;
	uint32_t BasePatchAddress;
	uint32_t PatchTableOffsetNumber;
	uint32_t PatchTableOffset;
};