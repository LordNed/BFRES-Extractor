# BFRES-Extractor
Extracts files from Nintendo/WiiU .bfres files used in Pikmin, Wind Waker, Mario Kart and more.

## What this tool does and doesn't do:
The only thing that mostly* works at this point is the extraction of "Embedded Files" (see [file spec](http://mk8.tockdom.com/wiki/BFRES_(File_Format))). There are 11 other sub-types for files and partial extraction of those is supported. More on this in a minute...

FTEX files now dump out both their FTEX header, as well as a .gtx file containing the actual texture data. Support for this was added in v0.2. Fairly untested, but seemed to work from what I could see.

## Usage:
Drag and drop an **uncompressed** .bfres file onto the executable. Folders for the 12 sub-file types will be generated alongside the .bfres file containing their contents. Many .bfres files can be found inside of [.sarc archives](http://mk8.tockdom.com/wiki/SARC_%28File_Format%29) - an extractor is available on this page.

![](http://i.imgur.com/Yl3Nd2D.png)

## Caveats and Current BFRES Understandings
Initial understanding of the BFRES format (based on documentation on TockDom) lead us to believe that it was an archive format for various game content. However, further research leads us to believe that this is not true.

The BFRES format is organized in the following fashion.

* [BFRESHeader]
* [Embedded Data Offset Pairs]
* [Index Groups]
* [Non-Embedded Data]
* [String Table]
* [Unknown]
* [Embedded Data]

So, the complicated part. If you run the tool on an archive at this point, you'll end up with not as much content as the file actually holds, yet there are the correct number of files and (to our knowledge) the files are of full length. So where is the missing data you ask?

Enter the [unknown] section above. It appears (at this early point in time) that some sub-types (such as FMDL) actually have references embedded within their files that point to this [unknown] block which stores data. It appears that FSHP and the actual bitmap texture data for models is contained within this [unknown] block (as well as others?). What this means is that A) The BFRES file is not a traditional archive format as the data is not grouped by which file it belongs to. It's more of an "optimized package" where data is strewn about the format belonging to various bits of it.

This is why I'm calling it a "partial" extractor - a better understanding (and more code) for the FMDL and various other sub-type sections will have to be documented before completed data can be extracted from the format.

## Other caveats / potential bugs
The math on the file extraction is a little... complicated right now. Offsets are given relative to their current position in the stream so we tend to have to jump around a bit and do some odd math (where we add the offset of the variable we're reading to the position in memory, etc.)

Of note, for [writing Embedded Files to Disk](https://github.com/LordNed/BFRES-Extractor/blob/master/BFRESExtractor/BFRESExtractor.cpp#L71) we have to add to their offset by (embeddedFileNumber * 0x8) to produce images without extra padding at the start of each file. This seems to work on all tested files (everyone seems to start with the right MAGIC) and was verified by extracting the jpegs shown above, but accurate analysis / testing is impossible without having readers for the various formats contained to ensure that we are actually getting all of the files every time.

Other potential cases for errors is in the [calculation of file lengths for sub-file types](https://github.com/LordNed/BFRES-Extractor/blob/master/BFRESExtractor/BFRESExtractor.cpp#L200). The length is not explicitly given, so we have to look at the offset of the next file and take the difference between the two. This seems to work in theory, but I might be off by 0x8 / 0xC here and there which could prove to be a huge pain later on in life if someone tries to RE a format extracted by this.

## Credits
Written by LordNed with perpetual help from Sage of Mirrors, and, as usual, our work is built on the backs of those who came before us on various wikis and previously written tools. None of this would be possible without disc dumping tools, archive extracting tools, and partial (if not full) wiki documentation for various things, and to those who write these you have our gratitude.

FTEX dumping support added as a result of work done by TwilightZoney and ItEasyActually. Thanks for walking me through the format and letting me know what bits and bobs I needed from various formats to get these dumped out!