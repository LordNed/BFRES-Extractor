#pragma once
#include <intrin.h>

struct IndexGroupNode
{
	int SearchValue; //A value used in binary search tree traversal
	unsigned short LeftIndex; //Left index used for BST
	unsigned short RightIndex; //Right index used for BST
	unsigned int NameOffset; //Offset to the name of this entry
	int DataOffset; //Offset to the data of this entry. Can be negative!

	int GetSearchValue()
	{
		return _byteswap_ulong(SearchValue);
	}

	unsigned short GetLeftIndex()
	{
		return _byteswap_ushort(LeftIndex);
	}

	unsigned short GetRightIndex()
	{
		return _byteswap_ushort(RightIndex);
	}

	unsigned int GetNameOffset()
	{
		int relOffset = _byteswap_ulong(NameOffset); //Relative to?
		return relOffset;
	}

	int GetDataOffset()
	{
		return _byteswap_ulong(DataOffset); // Relative to?
	}
};

struct IndexGroup
{
	unsigned int Length; // Length of the group in bytes
	unsigned int NodeCount; // Number in group (excluding the root node)

	unsigned int GetLength()
	{
		return _byteswap_ulong(Length);
	}

	unsigned int GetNodeCount()
	{
		return _byteswap_ulong(NodeCount);
	}

	IndexGroupNode *GetRootNode()
	{
		return (IndexGroupNode *)((char *)this + sizeof(IndexGroup));
	}

	IndexGroupNode *GetNode(unsigned int nodeIndex)
	{
		// Nodes start after the root node, so we add on the size of the group node.
		return (IndexGroupNode *)((char *)this + sizeof(IndexGroup) + (sizeof(IndexGroupNode) * (nodeIndex + 1)));
	}
};

struct FRESHeader
{
	unsigned char Magic[4]; //FRES
	char UnknownA;
	char UnknownB;
	char UnknownC;
	char UnknownD;
	short ByteOrderMark; //0xFE, 0xFF for big endian, 0xFF, 0xFE for little endian.
	short UnknownVersion; //Maybe a version number?
	unsigned int FileLength; //Total file length (header + contents) in bytes.
	unsigned int FileAlignment; // A power of 2 (?)
	unsigned int FileNameOffset; // File name offset (relative to this variable in memory, so add 0x14 from FRESHeader start to get correct offset)
	unsigned int StringTableLength; //Length? Offset? Seems to be offset to first file name in table... (Relative to end of header)
	unsigned int StringTableOffset; // Offset to a string table.
	unsigned int IndexGroupOffsets[12]; // Offsets to Index Groups for 12 subfile types. 0 indicates no subfile type present.
	unsigned short IndexFileCounts[12];  // Number of files in the Index Group offset. 0 indicates no subfile type present.
	unsigned int UnknownF; //Always 0 in MK8Ca


	char *GetFileName()
	{
		int relOffset = _byteswap_ulong(FileNameOffset);

		// The FileNameOffset is specified relative to the location of the FileNameOffset
		// in the header. This is what the 0x14 comes from.
		return (char *)this + 0x14 + relOffset;
	}

	unsigned int GetFileLength()
	{
		return _byteswap_ulong(FileLength);
	}

	unsigned int GetFileAlignment()
	{
		return _byteswap_ulong(FileAlignment);
	}

	unsigned int GetStringTableLength()
	{
		return _byteswap_ulong(StringTableLength);
	}

	unsigned int GetStringTableOffset()
	{
		return _byteswap_ulong(StringTableOffset);
	}

	unsigned int GetFileOffsetForGroup(unsigned int groupIndex)
	{
		// Returns the file offset relative to the end of the header
		return _byteswap_ulong(IndexGroupOffsets[groupIndex]);
	}

	IndexGroup *GetGroupForIndex(int index)
	{
		// This offset is given relative to the location of IndexGroupOffset[index]
		// in the file. 0x20 = offset of IndexGroupOffsets, then multiplied by
		// the index & size of each index to start from the right spot.
		int relOffset = GetFileOffsetForGroup(index);

		relOffset += 0x20 + (index * sizeof(unsigned long));
		return (IndexGroup *)((char *) this + relOffset);
	}

	unsigned short GetFileCountForGroup(int groupIndex)
	{
		return _byteswap_ushort(IndexFileCounts[groupIndex]);
	}
};