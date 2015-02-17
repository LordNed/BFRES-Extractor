// BFRESExtractor.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include "FRESFileFormat.h"
#include "FTexHeader.h"
#include "GTXFileFormat.h"
#include <Windows.h> // Needed for CreateDirectory(...)
#include <direct.h> // Needed for _getcwd 

static std::string CurrentWorkingDir;

static std::string GetFolderNameForGroupIndex(unsigned int index)
{
	switch (index)
	{
	case 0:
		return std::string("0_FMDL");
	case 1:
		return std::string("1_FTEX");
	case 2:
		return std::string("2_FSKA");
	case 3:
		return std::string("3_FSHU");
	case 4:
		return std::string("4_FSHU");
	case 5:
		return std::string("5_FSHU");
	case 6:
		return std::string("6_FTXP");
	case 7:
		return std::string("7_Unknown");
	case 8:
		return std::string("8_FVIS");
	case 9:
		return std::string("9_FSHA");
	case 10:
		return std::string("10_FSCN");
	case 11:
		return std::string("11_EmbeddedFiles");
	}

	return std::string("-1_UnknownIndex");
}

static void CreateDirectoryAbsolute(std::string directory)
{
	// There's nothing in the standard C++ libraries for creating a directory.
	// Options are "include windows.h" or "use boost". I chose the prior.

	CreateDirectory(directory.c_str(), NULL);
}

static void WriteEmbeddedFileToDisk(int nodeIndex, int nodeOffsetFromStartOfFile, IndexGroup *pSubFileData, FRESHeader *pHeader, char *pFileName)
{
	// Figure out the data offset location.
	int dataOffsetFromBoF = nodeOffsetFromStartOfFile + 0xC + pSubFileData->GetNode(nodeIndex)->GetDataOffset();
	int dataOffset = _byteswap_ulong(*(int *)((char *)pHeader + dataOffsetFromBoF));
	int dataLength = _byteswap_ulong(*(int *)((char *)pHeader + dataOffsetFromBoF + 0x4));

	std::cout << "\t\tEmbedded Data Offset: " << dataOffset << " Length: " << dataLength << std::endl;

	std::string outputDir = CurrentWorkingDir + GetFolderNameForGroupIndex(11);
	outputDir.append("\\");
	CreateDirectoryAbsolute(outputDir);
	outputDir.append(pFileName);

	std::filebuf fb;
	fb.open(outputDir, std::ios::out | std::ios::ate | std::ios::binary);

	std::ostream outFile(&fb);
	outFile.write((char *)pHeader + sizeof(FRESHeader) + dataOffset + (nodeIndex * 0x8), dataLength);
	fb.close();
}

static void WriteFTexDataToDisk(FRESHeader *pHeader, IndexGroup *pFTexGroup)
{
	for (int i = 0; i < pFTexGroup->GetNodeCount(); i++)
	{
		int nodeOffsetFromStartOfFile = ((char *)pFTexGroup->GetNode(i) - (char *)pHeader);
		int dataOffset = nodeOffsetFromStartOfFile + 0xc + pFTexGroup->GetNode(i)->GetDataOffset();

		FTEXHeader *pFTexHeader = (FTEXHeader *)((char *)pHeader + dataOffset);
		char *pFileName = (char *)pHeader + dataOffset + 168 + pFTexHeader->GetFilenameOffset();

		std::cout << "["<<i<<"] Dumping FTex to GTX file " << pFileName << ".gtx" << std::endl;
		//std::cout << "File Name Offset: " << pFTexHeader->GetFilenameOffset() << std::endl;
		//std::cout << "MipMap Offset: " << pFTexHeader->GetMipMapOffset() << std::endl;
		//std::cout << "ImageData Offset: " << pFTexHeader->GetImageDataOffset() << std::endl;
		//std::cout << "MipMap Data Offset" << pFTexHeader->GetMipMapDataOffset() << std::endl;

		// The FTEXHeader::GFDSurfaceData blob contains two bits of information we need.
		// The size of the ImageData is stored at 32 (dec) and the size of the MipMap Data is stored at 40 (dec)
		int imageDataBE = (int)pFTexHeader->GFDSurfaceData[32] | ((int)pFTexHeader->GFDSurfaceData[33] << 8) | ((int)pFTexHeader->GFDSurfaceData[34] << 16) | ((int)pFTexHeader->GFDSurfaceData[35] << 24);
		int imageDataSize = _byteswap_ulong(imageDataBE);

		int mipmapDataBE = (int)pFTexHeader->GFDSurfaceData[40] | ((int)pFTexHeader->GFDSurfaceData[41] << 8) | ((int)pFTexHeader->GFDSurfaceData[42] << 16) | ((int)pFTexHeader->GFDSurfaceData[43] << 24);
		int mipmapDataSize = _byteswap_ulong(mipmapDataBE);

		//std::cout << "ImageData Size: " << imageDataSize << std::endl;
		//std::cout << "Mipmap Data Size: " << mipmapDataSize << std::endl;

		// Time to write an GTX file to disk based on the extracted information. 
		std::string outputDir = CurrentWorkingDir + GetFolderNameForGroupIndex(1);
		outputDir.append("\\");
		CreateDirectoryAbsolute(outputDir);
		outputDir.append(pFileName);
		outputDir.append(".gtx");

		std::ofstream gtxFile;
		gtxFile.open(outputDir.c_str(), std::ios::out | std::ios::binary);

		GTXHeader gtxHeader;
		gtxHeader.Magic[0] = 'G'; gtxHeader.Magic[1] = 'f'; gtxHeader.Magic[2] = 'x'; gtxHeader.Magic[3] = '2'; // Please never do this again :D
		gtxHeader.HeaderSize = _byteswap_ulong(32);
		gtxHeader.MajorVersion = _byteswap_ulong(7);
		gtxHeader.MinorVersion = _byteswap_ulong(1);
		gtxHeader.GpuVersion = _byteswap_ulong(2);
		gtxHeader.AlignmentMode = 0;
		gtxHeader.Reserved1 = 0;
		gtxHeader.Reserved2 = 0;

		// Stuff it into our file.
		gtxFile.write((char *)&gtxHeader, sizeof(GTXHeader));

		GTXBlockHeader firstBlock;
		firstBlock.Magic[0] = 'B'; firstBlock.Magic[1] = 'L'; firstBlock.Magic[2] = 'K'; firstBlock.Magic[3] = '{'; // I just said not to do it!
		firstBlock.HeaderSize = _byteswap_ulong(32);
		firstBlock.MajorVersion = _byteswap_ulong(1);
		firstBlock.MinorVersion = 0;
		firstBlock.Type = _byteswap_ulong(11);
		firstBlock.DataSize = _byteswap_ulong(156);
		firstBlock.Id = 0;
		firstBlock.TypeIndex = 0;

		// Cram that into our file
		gtxFile.write((char *)&firstBlock, sizeof(GTXBlockHeader));

		// Apparently we now cram the entire 156 byte array from the FTEX header into the file. 
		gtxFile.write((char *)pFTexHeader->GFDSurfaceData, 156); 

		// Most of the stuff here is the same, so we'll copy...
		GTXBlockHeader imageDataHeader = firstBlock;
		imageDataHeader.Type = _byteswap_ulong(12);
		imageDataHeader.DataSize = _byteswap_ulong(imageDataSize);

		gtxFile.write((char *)&imageDataHeader, sizeof(GTXBlockHeader));

		// Now write the actual image data from the BFRES file into the GTX header.
		// 0xB4 gets us the offset from the start of the pFTexGroup and all of the offsets are relative to their
		// current positions.
		gtxFile.write((char *)pHeader + dataOffset + 0xB0 + pFTexHeader->GetImageDataOffset(), imageDataSize);


		// Copy again
		GTXBlockHeader mipmapDataHeader = imageDataHeader;
		mipmapDataHeader.Type = _byteswap_ulong(13);
		mipmapDataHeader.DataSize = _byteswap_ulong(mipmapDataSize);

		gtxFile.write((char *)&mipmapDataHeader, sizeof(GTXBlockHeader));

		// Stuff mipmap data in.
		gtxFile.write((char *)pHeader + dataOffset + 0xB4 + pFTexHeader->GetMipMapDataOffset(), mipmapDataSize);

		// And finally a 'null' GTXBlockHeader to signify file end.
		GTXBlockHeader terminator = mipmapDataHeader;
		terminator.Type = _byteswap_ulong(1);
		terminator.DataSize = 0;

		gtxFile.write((char *)&terminator, sizeof(GTXBlockHeader));
		gtxFile.close();
	}
}

int main(int argc, char* argv[])
{
	// Check that we got passed a file
	if (argc < 2)
	{
		std::cout << "Usage: bfresextractor \"c:\\path\\to\\file.bfres\"!" << std::endl;
		system("pause");
		return -1;
	}

	std::string filePath = argv[1];

	// Get and store the current working directory for output operations.
	char currentPath[FILENAME_MAX];
	_getcwd(currentPath, sizeof(currentPath));

	CurrentWorkingDir = std::string(currentPath);
	CurrentWorkingDir.append("\\");

	std::cout << "Current working dir (Output Path): " << CurrentWorkingDir.c_str() << std::endl;

	std::ifstream file;
	file.open(filePath, std::ifstream::ate | std::ifstream::binary);
	if (file.is_open())
	{
		std::streampos size = file.tellg();

		char *pMemory = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(pMemory, size);


		FRESHeader *pHeader = (FRESHeader*)pMemory;
		std::cout << "File Name: " << pHeader->GetFileName() << std::endl;
		std::cout << "File Length: " << pHeader->GetFileLength() << " bytes [0x" << std::hex << pHeader->GetFileLength() << std::dec << "]" << std::endl;
		std::cout << "Alignment Offset: " << pHeader->GetFileAlignment() << std::endl;
		std::cout << "String Table Offset: " << pHeader->GetStringTableOffset() << " bytes [0x" << std::hex << pHeader->GetStringTableOffset() << std::dec << "]" << std::endl;
		std::cout << "String Table Length: " << pHeader->GetStringTableLength() << " bytes [0x" << std::hex << pHeader->GetStringTableLength() << std::dec << "]" << std::endl;

		std::cout << "Index Group Data: " << std::endl;
		for (int i = 0; i < 12; i++)
		{
			std::cout << "[" << i << "] Count: " << pHeader->GetFileCountForGroup(i) << " Offset: " << pHeader->GetFileOffsetForGroup(i) << " [0x" << std::hex << pHeader->GetFileOffsetForGroup(i) << std::dec << "]" << std::endl;
		}

		for (int subFileType = 0; subFileType < 12; subFileType++)
		{
			if (pHeader->GetFileOffsetForGroup(subFileType) == 0)
			{
				std::cout << "No files for sub-file type found, skipping..." << std::endl;
				continue;
			}
			else
			{
				std::cout << "Now processing sub-file type: " << subFileType << "..." << std::endl;
			}

			IndexGroup *pSubFileData = pHeader->GetGroupForIndex(subFileType);
			std::cout << "-Length: " << pSubFileData->GetLength() << " Node Count: " << pSubFileData->GetNodeCount() << std::endl;
			std::cout << "-Root Node" << std::endl;
			std::cout << "-Search Value: " << pSubFileData->GetRootNode()->GetSearchValue() << std::endl;
			std::cout << "-Left Index: " << pSubFileData->GetRootNode()->GetLeftIndex() << std::endl;
			std::cout << "-Right Index: " << pSubFileData->GetRootNode()->GetRightIndex() << std::endl;
			std::cout << "-Name Offset: " << pSubFileData->GetRootNode()->GetNameOffset() << std::endl;
			std::cout << "-Data Offset: " << pSubFileData->GetRootNode()->GetDataOffset() << std::endl;

			for (int i = 0; i < pSubFileData->GetNodeCount(); i++)
			{
				// Figure out the name.
				int nodeOffsetFromStartOfFile = ((char *)pSubFileData->GetNode(i) - (char *)pHeader);

				// 0x8 = Offset within the node to where the GetNameOffset is relative to.
				char *pFileName = (char *)pHeader + nodeOffsetFromStartOfFile + 0x08 + pSubFileData->GetNode(i)->GetNameOffset();

				std::cout << "-\tNode: " << i << " [0x" << std::hex << nodeOffsetFromStartOfFile << std::dec << " | " << nodeOffsetFromStartOfFile << "]" << std::endl;
				std::cout << "-\t\tSearch Value: " << pSubFileData->GetNode(i)->GetSearchValue() << std::endl;
				std::cout << "-\t\tLeft Index: " << pSubFileData->GetNode(i)->GetLeftIndex() << std::endl;
				std::cout << "-\t\tRight Index: " << pSubFileData->GetNode(i)->GetRightIndex() << std::endl;
				std::cout << "-\t\tName Offset: " << pSubFileData->GetNode(i)->GetNameOffset() << " [" << pFileName << "]" << std::endl;
				std::cout << "-\t\tData Offset: " << pSubFileData->GetNode(i)->GetDataOffset() << std::endl;

				// For sub-file types 0-10 they use a standard method of embedding the data format. This means that the data is offset
				// after the end of the node, and everything is dandy. However, for sub-file type 11 it has offsets which are negative
				// and point towards the start of a file which explicitly lists their offset and length. I have no idea why they're 
				// different, but the end result is is that we handle these two cases differently.
				if (subFileType < 11)
				{
					int dataOffset = nodeOffsetFromStartOfFile + 0xc + pSubFileData->GetNode(i)->GetDataOffset();

					// Calculating the length is interesting. In theory, it uses the distance between the current node's data offset
					// and the next node's data offset. However, there can be cases where there is only one node, so we're pretty sure
					// it either uses the StringTable offset, or the UnknownE offset from the top. (No one really knows! :D)

					int dataLength = 0;
					if (i < pSubFileData->GetNodeCount() - 1)
					{
						// This case is an easy one. We're not the last node in this category,
						// so we'll just use the next one.
						int curFileRelStart = (pSubFileData->GetNode(i)->GetDataOffset() + nodeOffsetFromStartOfFile + 0xC);

						int nextNodeOffsetFromStartOfFile = ((char *)pSubFileData->GetNode(i + 1) - (char *)pHeader);
						int nextFileRelStart = (pSubFileData->GetNode(i + 1)->GetDataOffset() + nextNodeOffsetFromStartOfFile + 0xC);

						dataLength = nextFileRelStart - curFileRelStart;
					}
					else
					{
						// Okay, so this one is a bit harder. We're the last node in the array, so we need to get the next node, from the next array that has something.
						// But, if we never find anything, then we have to use the StringTableOffset/UnknownE offset (unsure yet). We also never want to look in the 11
						// sub-filetype as that suddenly points to embedded files which are *not* organized the same way.
						for (int subNode = subFileType + 1; subNode < 11; subNode++)
						{
							//  Skip empty sub-groups.
							if (pHeader->GetFileCountForGroup(subNode) == 0)
								continue;

							// If we've reached here then the sub-node does have a file in it and we're just going to use the first
							// and then break out.
							int curFileRelStart = (pSubFileData->GetNode(i)->GetDataOffset() + nodeOffsetFromStartOfFile + 0xC);
							int nextFileRelStart = (pHeader->GetGroupForIndex(subNode)->GetNode(0)->GetDataOffset() + nodeOffsetFromStartOfFile + 0xC);


							dataLength = pHeader->GetGroupForIndex(subNode)->GetNode(0)->GetDataOffset() - pSubFileData->GetNode(i)->GetDataOffset();
							break;

							//@ToDo: This is probably wrong, see above.
						}

						// If dataLength is still zero at this point then we're looking at the very last (non-embedded) file in the archive. We'll
						// guess and use the StringTableOffset/UnknownE as their length.
						if (dataLength == 0)
						{
							// Adding nodeOffsetFromStartOfFile and 0xC converts the GetDataOffset() (relative) to from start of file.
							dataLength = pHeader->GetStringTableOffset()- (pSubFileData->GetNode(i)->GetDataOffset() + nodeOffsetFromStartOfFile + 0xC);
						}
					}

					if (dataLength == 0)
					{
						std::cout << "File broke our logic, expect nothing to work/crashes!" << std::endl;
					}

					// Yay, we can write this to file now.
					std::string outputDir = CurrentWorkingDir + GetFolderNameForGroupIndex(subFileType);
					outputDir.append("\\");
					CreateDirectoryAbsolute(outputDir);
					outputDir.append(pFileName);

					std::filebuf fb;
					fb.open(outputDir, std::ios::out | std::ios::ate | std::ios::binary);

					std::ostream outFile(&fb);
					outFile.write((char *)pHeader + dataOffset, dataLength);
					fb.close();
				}
				else
				{
					// If we're looking at the Embedded File sub-type then the data is stored differently. We'll handle them seperately.
					WriteEmbeddedFileToDisk(i, nodeOffsetFromStartOfFile, pSubFileData, pHeader, pFileName);
				}

			}

			// We're now going to process as much sub-file data. This would be where you'd want to add an extractor for another
			// type of data once the file format is mostly mapped.
			switch (subFileType)
			{
				case 0: // FMDL
					break;
				case 1: // FTEX
					WriteFTexDataToDisk(pHeader, pSubFileData);
					break;
				case 2: // FSKA (Skeleton Animation?)
				case 3: 
				case 4:
				case 5: // FSHU (Shape...?)
				case 6: // FTXP data (?)
				case 7: // Unknown
				case 8: // FVIS (Bone Visibility)
				case 9: // FSHA (Shape?)
				case 10: // FSCN (Scene/Hiearchy?)
				case 11: // Embedded File (Handled above)
					break;
			}
		}


		delete[] pMemory;
	}
	file.close();

	system("pause");
	return 0;
}
