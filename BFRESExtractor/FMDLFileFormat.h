#pragma once
#include <intrin.h>

struct ShadowParameter
{
	// Todo
};

struct ShaderControl
{
	// Todo.
};

struct UnknownMaterialStructure
{
	int UnknownA; // Always less than 0x14
	short UnknownB; //Always 0x0028
	short UnknownC; // 0x0240, 0x0242, 0x0243
	int UnknownD; //0x49749732 or 0x49749736
	int UnknownE; // Always less than 0x00cc
	float UnknownF; // Always less than 1.0
	short UnknownG; //Always 0x00CC
	short UnknownH; //Always 0x000000 or 0x0100
	int UnknownI; //Always 0
	short UnknownJ; //Always 0x2001
	char UnknownK; //Always 1 or 5 
	char UnknownL; // Always 1 or 4
	int UnknownM[4]; //16 bytes, always 0.
};

// Uniform Variables
struct MaterialParameter
{
	char Type; // Type of this variable
	char Size; // Size of value in bytes
	short Offset; // Offset relative to the start of the MaterialParameterData section to the value.
	int UnknownA; // Always 0xFFFFFFFF
	int UnknownB; // Always 0
	short SectionIndex; // Index into the Material Parameter array.
	int VariableNameOffset; // Offset of the Variable Name

	// Potential values for the Type field.

//	Type	Size	Description
//		0x04	4	Signed Integer.
//		0x0c	4	Floating point value.
//		0x0d	8	Vector of 2 floats.
//		0x0e	12	Vector of 3 floats.
//		0x0f	16	Vector of 4 floats.
//		0x1e	24	2×3 Texture Matrix. Encoded down columns then along rows, so the identity matrix would be encoded 1, 0, 0, 1, 0, 0.

};

// Describes an input to a pxiel shader from the selected texture.
struct TextureAttributeSelector
{
private:
	char UnknownA; // Always 2
	char UnknownB; // 0x0, 0x2, 0x4, or 0x12
	char UnknownC; // 0x0, 0x10, 0x12, 0x5a
	char UnknownD; // Either near zero or near 0x80. Flags?
	char UnknownE; // Signed integer normally close to 0
	char UnknownF; // Typically a small value
	short UnknownG; // Probably flags
	int UnknownH; // Always 0x80000000
	int UnknownI; // Always 0
	int AttributeNameOffset; 
	char SectionIndex; // Index of this element
	char Padding[3]; // Always 0

	// Potential Attribue Names and description

	//Attribute Name	Friendly Name	Description
	//	_a0					albedo0			Ordinary colour textures for surface albedo.
	//	_a1					albedo1
	//	_a2					albedo2
	//	_a3					albedo3
	//	_s0					specular0		Specular highlight texture.
	//	_n0					normal0			Normal map texture for altering surface normals.
	//	_n1					normal1
	//	_e0					emission0		Emissive lighting texture.
	//	_b0					bake0			Shadow textures.
	//	_b1					bake1
};

struct TextureSelector
{
private:
	int TextureNameOffset; // Same as the FTEX name
	int FTEXOffset; // Points to the header of the FTEX.
};

struct RenderInfoParameter
{
private:
	short UnknownA; // Always 0 or 1
	char Type; // Type of the variable
	char UnknownB; // Always 0
	int VariableNameOffset; // Offset to the variable name.
	void *Value; // Value of the variable.

	// Possible Type values. Non-exclusive list.
	//Type	Value Size	Description
	//0x00	    8	Unknown data always 0 ?
	//0x01	    8	Unknown vector of 2 floats ?
	//0x02	    4	Offset to a string.
};

struct FMATHeader
{
private:
	char Magic[4]; //FMAT
	int MaterialNameOffset;
	int UnknownA; //Always 1
	unsigned short SectionIndex; //Index into the FMAT array this is.
	unsigned short RenderInfoParameterCount; // Number of elemeents in the RenderInfoParameter group.
	unsigned char TextureSelectorCount; // Number of elements in the Texture Selector array.
	unsigned char TextureAttributeSelectorCount; // Number of elements in the Texture Attribute Selector array. Always equal to # above.
	unsigned short MaterialParameterCount; // Number of elements in the Material Parameter array.
	unsigned int MaterialParameterDataSize; //Size in bytes of the Material Parameter Data section
	int UnknownB; // values are 0, 1, or 2.
	int RenderInfoParameterIndexGroupOffset; //Offset to the Index Group defining render info parameters.
	int UnknownMaterialStructureOffset; // Offset to the unknown material structure.
	int ShaderControloffset; // Offset to the shader control structure.
	int TextureSelectorOffset; // offset ot the first element in the Texture Selector array.
	int TextureAttributeSelectorOffset; // Offset to the first element in the Texture Attribute Selector array.
	int TextureAttributeSelectorIndexGroupOffset; // Offset to an index group which references the Texture Attribute Selector array to allow named value lookups.
	int MaterialParameterArrayOffset; // Offset to the first element in the Material Parameter array.
	int MaterialParameterIndexGroupOffset; // Offset to an index group which references the Mateiral Parameter array to allow named valued ookups.
	int MaterialParameterDataOffset; // Offset to the beginning of the values for Mateiral Parameters. The Material Parameter array references this data.
	int ShadowParameterIndexGroupOffset; // May be 0 if nto needed
	int UnknownCOffset; // Always points to 12 bytes, all 0. Offset may be 0 if not needed.
};

struct FVTXBuffer
{
private:
	unsigned int UnknownA; //Always 0
	unsigned int Size; // Size of the buffer in bytes
	unsigned int UnknownB; //Always 0
	unsigned short Stride; // Size of each element in the buffer
	unsigned short UnknownC; // Always 1
	unsigned int UnknownD; // Always 0
	int DataOffset; 

public:
	unsigned int GetUnknownA()
	{
		return _byteswap_ulong(UnknownA);
	}

	unsigned int GetSize()
	{
		return _byteswap_ulong(Size);
	}

	unsigned int GetUnknownB()
	{
		return _byteswap_ulong(UnknownB);
	}

	unsigned short GetStride()
	{
		return _byteswap_ushort(Stride);
	}

	unsigned short GetUnknownC()
	{
		return _byteswap_ushort(UnknownC);
	}

	unsigned int GetUnknownD()
	{
		return _byteswap_ulong(UnknownD);
	}

	int GetDataOffset()
	{
		return _byteswap_ulong(DataOffset);
	}
};

struct FVTXAttribute
{
private:
	int AttributeNameOffset;
	char BufferIndex; // Index of the buffer containing this attribute.
	char BufferOffset[3]; // "Buffer offset of the attribute in each element in the buffer" (wat)
	unsigned int Format; // Format of the attribute's data in the buffer.

	// Possible values (Non-Exclusive list)
	// Value   Data Size   Description
	//0x00000004	2	Two 8 bit values representing numbers between 0 and 1.
	//0x00000007	4	Two 16 bit values representing numbers between 0 and 1.
	//0x0000020a	4	Four signed 8 bit values.
	//0x0000020b	4	Three signed 10 bit values.Divide by 511 to get decimal value.Top 2 bits are always 0b01.
	//0x0000080d	8	Two 32 bit floating point values.
	//0x0000080f	8	Four 16 bit floating point values.
	//0x00000811	12	Three 32 bit floating point values.

public:
	int GetAttributeNameOffset()
	{
		return _byteswap_ulong(AttributeNameOffset);
	}

	char GetBufferIndex()
	{
		return BufferIndex;
	}

	char GetBufferOffset(int index)
	{
		return BufferOffset[index]; //@ToDo: No idea what this one is, or if it's even right.
	}

	unsigned int GetFormat()
	{
		return _byteswap_ulong(Format);
	}
};

struct FVTXHeader
{
private:
	char Magic[4]; // FVTX
	unsigned char AttributeCount; // Number of different attributes (Position, Normal, Color, etc.) in Attribute Array
	unsigned char BufferCount; //Number of attribute buffers in Buffer array.
	unsigned short SectionIndex; //Index into FVX array that this entry is.
	unsigned int VertexCount; // Number of vertices, also equal to the number of elements in each buffer.
	unsigned char UnknownA; // Values between 0 and 4, usually 0?
	unsigned char UnknownB[3]; // Usually zero - padding?
	unsigned int AttributeArrayOffset; //Offset to the first element in the Attribute array.
	unsigned int AttributeIndexGroupOffset; // Named pointers to member of the Attribute Array to allow named lookups.
	unsigned int BufferArrayOffset; // Offset to the first element in the Buffer array.
	unsigned int Padding; // Always zero

public:
	unsigned char GetAttributeCount()
	{
		return AttributeCount;
	}

	unsigned char GetBufferCount()
	{
		return BufferCount;
	}

	unsigned short GetSectionIndex()
	{
		return _byteswap_ushort(SectionIndex);
	}

	unsigned int GetVertexCount()
	{
		return _byteswap_ulong(VertexCount);
	}

	unsigned char GetUnknownA()
	{
		return UnknownA;
	}

	unsigned int GetAttributeArrayOffset()
	{
		return _byteswap_ulong(AttributeArrayOffset);
	}

	unsigned int GetAttributeIndexGroupOffset()
	{
		return _byteswap_ulong(AttributeIndexGroupOffset);
	}

	unsigned int GetBufferArrayOffset()
	{
		return _byteswap_ulong(BufferArrayOffset);
	}
};

struct FMDLHeader
{
private:
	char Magic[4]; //FMDL;
	unsigned int FileNameOffset; // Offset to the file name (without file extension)
	unsigned int BFRESStringTableEnd; // Offset to the end of the BFRES string table.
	unsigned int FSKLOffset; // Offset to the FSKL header
	unsigned int FVTXOffset; // Offset to the FVertex header
	unsigned int FSHPIndexGroupOffset; // Offset to an index group which contains named pointers to FSHP Headers
	unsigned int FMATIndexGroupOffset; // Offset to an index group which contains named pointers to FMAT Headers
	unsigned int ParameterIndexGroupOffset; // Offset to an index group which contains named pointers to parameter values. May be 0 if no parameters exist (common)
	unsigned short FVTXCount; // Number of FVTX sections in the FVTXArray.
	unsigned short FSHPCount; // Number of FSHP sections in the FSHP index group.
	unsigned short FMATCount; // Number of FMAT sections in the FMAT index group
	unsigned short ParameterCount; // Number of parameters in the Parameter index group.
	unsigned int Unknown;

public:
	unsigned int GetFileNameOffset()
	{
		return _byteswap_ulong(FileNameOffset);
	}

	unsigned int GetBFRESStringTableEnd()
	{
		return _byteswap_ulong(BFRESStringTableEnd);
	}

	unsigned int GetFSKLOffset()
	{
		return _byteswap_ulong(FSKLOffset);
	}

	unsigned int GetFVTXOffset()
	{
		return _byteswap_ulong(FVTXOffset);
	}

	unsigned int GetFSHPIndexGroupOffset()
	{
		return _byteswap_ulong(FSHPIndexGroupOffset);
	}

	unsigned int GetFMATIndexGroupOffset()
	{
		return _byteswap_ulong(FMATIndexGroupOffset);
	}

	unsigned int GetParameterIndexGroupOffset()
	{
		return _byteswap_ulong(ParameterIndexGroupOffset);
	}

	unsigned short GetFVTXCount()
	{
		return _byteswap_ushort(FVTXCount);
	}

	unsigned short GetFSHPCount()
	{
		return _byteswap_ushort(FSHPCount);
	}

	unsigned short GetFMATCount()
	{
		return _byteswap_ushort(FMATCount);
	}

	unsigned short GetParameterCount()
	{
		return _byteswap_ushort(ParameterCount);
	}

	unsigned int GetUnknown()
	{
		return _byteswap_ulong(Unknown);
	}
};
