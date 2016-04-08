//=======================================================================
// PassMark SysInfo DLL
//=======================================================================

/*=======================================================================
PassMark SysInfo DLL

Copyright (c) 2010
	This software is the confidential and proprietary information of PassMark
	Software Pty. Ltd. ("Confidential Information").  You shall not disclose such
	Confidential Information and shall use it only in accordance with the terms
	of the license agreement you entered into with PassMark Software.

Program:
	SysInfo DLL

Module:
	SysInfoPassMark.h

Author(s):
	IR

Description:
	Header file for SysInfo DLL - specifc to DLL functions that will only be 
	avaialble to PassMark applications

History:
	04 May 2010:	Created.
=======================================================================*/

#define SYSINFO_API extern "C" __declspec(dllexport)

#include <windows.h>
#include <tchar.h>
#include <windows.h>
#include <winioctl.h>
#include <ntddvol.h>
#include "sysinfo.h"

/////////////////////////////////////////////////////////////////////////////////////////
// Directio support
/////////////////////////////////////////////////////////////////////////////////////////
//SysInfoDll_InitDirectIO
//
// Allow DirectIO.sys to be opened and a handle passed back to the calling application.
// This is to allow direct applciation access to DirectIO from the calling applcaition 
// with the SysInfo library is the single point where the DirectIO driver is loaded.
//
SYSINFO_API bool SysInfoDll_IsDirectIoInitalized();
SYSINFO_API BOOL SysInfoDll_InitDirectIO_named(HANDLE *hDirectIO, wchar_t *szDirectIOName);
SYSINFO_API BOOL SysInfoDll_InitializeDirectIoPP_named(HANDLE *hDirectIO, DWORD dwLPTNumber, int *IOAddr, wchar_t *szDirectIOName);
SYSINFO_API BOOL SysInfoDll_InitDirectIO(HANDLE *hDirectIO);
SYSINFO_API BOOL SysInfoDll_InitializeDirectIoPP(HANDLE *hDirectIO, DWORD dwLPTNumber, int *IOAddr);
SYSINFO_API BOOL SysInfoDll_CloseDirectIO();

/////////////////////////////////////////////////////////////////////////////////////////
// Partition table information for PassMark Only applciations
/////////////////////////////////////////////////////////////////////////////////////////
#define MAX_PARTITIONS 256		//Arbitrary number
int SysInfoDll_GetPartitionTableLocations(HANDLE hDrive, WORD wDrive, LARGE_INTEGER liPartionTable[MAX_PARTITIONS], int *piPT);

/////////////////////////////////////////////////////////////////////////////////////////
// Unallocated cluster
/////////////////////////////////////////////////////////////////////////////////////////
// UnallocatedClusters module
// (c) Copyright PassMark Software 2009
//
// Author(s):
//    Keith Mah, Raymond Leung
//
// This class provides access and information on unallocated clusters for
// the given disk.

#ifndef UNALLOCATED_CLUSTERS_H
#define UNALLOCATED_CLUSTERS_H

#define BUF_SIZE 512
#define _MAX_PATH   260 /* max. length of full pathname */
#define MAX_FILE 512
#define	VLONG_STRING_LEN		1024	//Size of a generic very long string	//IR1

/*************************
 * Structs
 *************************/
typedef struct DiskBlock_s {
	UINT64 startLCN;
	UINT64 blockSize;
	BOOL occupied;
} DiskBlock_t;

typedef struct {
    VOLUME_DISK_EXTENTS volExtents;
    DISK_EXTENT extent;
} MIRRORED_DISK_EXTENT;

//
// template for creating aligned data structures
// for unaligned arrays of bytes.
//
template <typename T> struct UNPACK_TYPE {
    union {
        UCHAR byteArray[sizeof(T)];
        T     alignedType;
    };
};
//
// macro to instantiate an instance of an UNPACK_TYPE
// type and use it to copy from an unaligned array of bytes
// to an aligned integral type
//
#define COPY_OP(U_T, DST, SRC) \
    U_T * ucD = reinterpret_cast<U_T *>(DST);               \
    const U_T * ucS = reinterpret_cast<const U_T *>(SRC);   \
    *ucD = *ucS;
//
// template function to perform a copy from an unaligned array of
// bytes into an integral type.
//
template <typename T> void unalignedCopy(T * Dst, const UCHAR* Src)
{
    COPY_OP(UNPACK_TYPE<T>, Dst, Src);
}

//
// This is the on disk layout of the FAT32 BPB. You can find
// many versions of it defined on the web. I think this one is
// correct. Too bad there doesn't appear to be a spec for this.
//
struct OndiskFat32Bpb {
    UCHAR   BytesPerSector[2];
    UCHAR   SectorsPerCluster[1]; 
    UCHAR   ReservedSectors[2]; 
    UCHAR   NumberOfFats[1];
    UCHAR   RootEntries[2];
    UCHAR   NumberOfSectors[2];
    UCHAR   MediaDescriptor[1];
    UCHAR   SectorsPerFat[2];
    UCHAR   SectorsPerTrack[2]; 
    UCHAR   HeadsPerCylinder[2];
    UCHAR   HiddenSectors[4];
    UCHAR   BigNumberOfSectors[4];
    UCHAR   BigSectorsPerFat[4];
    UCHAR   ExtFlags[2];
    UCHAR   FsVersion[2];
    UCHAR   RootDirectoryStart[4];
    UCHAR   FsInfoSector[2];
    UCHAR   BackupBootSector[2];
    UCHAR   Reserved[12];                
};

// this below is a processor friendly version of the FAT32 BPB
// from http://www.wd-3.com/archive/luserland.htm
struct Fat32Bpb 
{
    USHORT  BytesPerSector;
    UCHAR   SectorsPerCluster; 
    USHORT  ReservedSectors; 
    UCHAR   NumberOfFats;
    USHORT  RootEntries;
    USHORT  NumberOfSectors;
    UCHAR   MediaDescriptor;
    USHORT  SectorsPerFat;
    USHORT  SectorsPerTrack; 
    USHORT  HeadsPerCylinder;
    ULONG   HiddenSectors;
    ULONG   BigNumberOfSectors;
    ULONG   BigSectorsPerFat;
    USHORT  ExtFlags;
    USHORT  FsVersion;
    ULONG   RootDirectoryStart;
    USHORT  FsInfoSector;
    USHORT  BackupBootSector;
    //
    // copy constructor from ondisk format to in-memory format
    //
    Fat32Bpb(const OndiskFat32Bpb& rhs)
    {
        unalignedCopy( &BytesPerSector,     rhs.BytesPerSector );
        unalignedCopy( &SectorsPerCluster,  rhs.SectorsPerCluster );
        unalignedCopy( &ReservedSectors,    rhs.ReservedSectors );
        unalignedCopy( &NumberOfFats,       rhs.NumberOfFats );
        unalignedCopy( &RootEntries,        rhs.RootEntries);
        unalignedCopy( &NumberOfSectors,    rhs.NumberOfSectors);
        unalignedCopy( &MediaDescriptor,    rhs.MediaDescriptor);
        unalignedCopy( &SectorsPerFat,      rhs.SectorsPerFat);
        unalignedCopy( &SectorsPerTrack,    rhs.SectorsPerTrack);
        unalignedCopy( &HeadsPerCylinder,   rhs.HeadsPerCylinder);
        unalignedCopy( &HiddenSectors,      rhs.HiddenSectors);
        unalignedCopy( &BigNumberOfSectors, rhs.BigNumberOfSectors);
        unalignedCopy( &BigSectorsPerFat,   rhs.BigSectorsPerFat);
        unalignedCopy( &ExtFlags,           rhs.ExtFlags);
        unalignedCopy( &FsVersion,          rhs.FsVersion);
        unalignedCopy( &RootDirectoryStart, rhs.RootDirectoryStart);
        unalignedCopy( &FsInfoSector,       rhs.FsInfoSector);
        unalignedCopy( &BackupBootSector,   rhs.BackupBootSector);
    }
};
//
// this provides a sector sized object
// for reading the boot sector off of the volume,
// and getting access to the OndiskFat32Bpb.
//
struct FAT32_BOOT_SECTOR 
{
    union {
        UCHAR sector[512];
        struct {
            UCHAR reserved[11];
            OndiskFat32Bpb ondiskBpb;
        } bpb;
    };
};

/*
#define UNALLOC_CLUSTERS_FILESYSTEM_NTFS	0
#define	UNALLOC_CLUSTERS_FILESYSTEM_FAT32	1
#define	UNALLOC_CLUSTERS_FILESYSTEM_FAT		2

#define UNALLOC_CLUSTERS_LOOKAHEAD_COUNT	10

#define UNALLOC_CLUSTERS_NODISKNUMBER		4294967295	// max_int (or DWORD value of -1)

class SysInfoDll_UnallocatedClusters	//SI101008
{
public:
	SysInfoDll_UnallocatedClusters(TCHAR* driveBuf);
	virtual ~SysInfoDll_UnallocatedClusters();

	bool Initialize();	
	bool OpenDisk(DWORD diskNumber, TCHAR * szFN);
	
	bool GetNextClusterBlock(BOOL bOccupied, UINT64* retStartCluster, UINT64* retEndCluster, UINT64* numClusters);	
	UINT64 ReadClusterBlock(UINT64 startCluster, UINT64 numClusters, BYTE* destBuffer, UINT64 bufferSize, UINT64* bytesRead);
	
	bool TranslateLogicalToPhysical(UINT64 inLcn, UINT64* outPhysicalOffset, ULONG* outDiskNumber);
	
	void		ResetClusterBlockToStart();																		
	ULONGLONG	SysInfoDll_UnallocatedClusters::NumberOfClustersAllocated(UINT64 ullStartCluster, UINT64 ullNumClusters)	;

	DWORD clusterSize;
	UINT64 totalClusters;

private:
	BOOL GetFat32Offsets(HANDLE hVolume);

	HANDLE hVolume;
	HANDLE hDisk;
	DWORD CurrentDiskNumber;

	int fileSystem;

	//TCHAR driveStr2[_MAX_PATH+1];
	TCHAR driveStr[_MAX_PATH+1];

	TCHAR tmpbuffer[BUF_SIZE];
	UINT64 maxClusters;
	UINT64 totalFreeClusters;
	UINT64 totalOccupiedClusters;
	UINT64 maxFreeConsecutiveClusters;
	UINT64 maxOccupiedConsecutiveClusters;
	
	DiskBlock_t* maxFreeClusters;
	UINT64 maxFreeClustersLen;

	UINT64 index, minIndex;
	
	DWORD sectorsPerCluster, bytesPerSector, numberOfFreeClusters,totalNumberOfClusters;

	// Volume cluster bitmap parameters 
	STARTING_LCN_INPUT_BUFFER startLcn;
	VOLUME_BITMAP_BUFFER* volumeBMPBuffer;

	UINT64 bitIdx;
	UINT64 clusterIdx;

	int ReadBufferLimit;

	TCHAR fileSystemName[MAX_PATH];

	UINT64 physicalOffsetLookAhead;
	DWORD diskNumberLookAhead;
	bool bHasLookAhead;
	int lookAheadCount;

	DWORD rootStart;
	DWORD clusterStart;

};
*/


#endif