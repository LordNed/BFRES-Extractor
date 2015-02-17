#pragma once

struct FTEXHeader
{
	unsigned char Magic[4]; //FTEX
	unsigned char GFDSurfaceData[156];
	unsigned int Unknown[2]; // 0x00000000 and 1
	unsigned int FilenameOffset;
	unsigned int MipMapOffset;
	unsigned int ImageDataOffset;
	unsigned int MipMapDataOffset;
	unsigned int Padding[2];

	int GetFilenameOffset()
	{
		return _byteswap_ulong(FilenameOffset);
	}

	int GetMipMapOffset()
	{
		return _byteswap_ulong(MipMapOffset);
	}

	int GetImageDataOffset()
	{
		return _byteswap_ulong(ImageDataOffset);
	}

	int GetMipMapDataOffset()
	{
		return _byteswap_ulong(MipMapDataOffset);
	}
};