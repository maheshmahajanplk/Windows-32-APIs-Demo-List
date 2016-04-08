//=======================================================================
// PassMark SysInfo DLL
//=======================================================================

/*=======================================================================
PassMark SysInfo DLL

	The PassMark SysInfo DLL provides a simple mechanism to get and format 
	System Information. Included is a set of APIs for retrieving SMART
	information from hard disks, which were formerly packaged as a 
	separate SDK called SmartDisk DLL SDK.

Copyright (c) 2010
	This software is the confidential and proprietary information of PassMark
	Software Pty. Ltd. ("Confidential Information").  You shall not disclose such
	Confidential Information and shall use it only in accordance with the terms
	of the license agreement you entered into with PassMark Software.

Specific Disclaimer on SMART Failure Predictions
	The SysInfo DLL SDK provides utilises statistical analysis to predict possible failure
	dates of hard disk drives. Because there are no ‘certainties’ in statistical
	analysis, PassMark® Software Pty Ltd disclaims all liability for any and all
	costs incurred by either:

	1)	The hard disk drive failing before the predicted failure date estimated
		by the application, or
	2)	The hard disk drive continuing to function beyond the predicted failure
		date estimated by the application.

	In either situation, PassMark® disclaims liability for any losses due to loss
	or damage to data. PassMark® further disclaims any liability for costs incurred
	in anticipation of a disk drive failure that does not eventuate (e.g. replacement
	hard disk drives, transfer time, downtime, etc).

	This disclaimer is in addition to the Disclaimer of Warranty and Limitation of
	Liability mentioned elsewhere in this EULA.

Program:
	SysInfo DLL

Module:
	SysInfo.h

Author(s):
	SK, IR, KM

Description:
	Header file for SysInfo DLL.

History:
	 2 Sep 2010:	Improvements to the System information interfaces and reports.
	10 Aug 2010:	Added new APIs for retrieving memory SPD information
	24 Jun 2010:	Added support for obtaining SMART information from USB hard disks
					Changed the DEVICEINFO struct to include attributes defined in ATA/ATAPI Command Set - 2 (ACS-2)
					Removed SysInfoDll_Smart_GetDeviceVendorUnique because the field is obsolete
	04 May 2010:	Changed the DLL from a SMART information DLL to a broader 
					System information DLL, retaining the SMART functions.
=======================================================================*/

#ifndef _SYSINFO_H_
#define _SYSINFO_H_

#define SYSINFO_API extern "C" __declspec(dllexport)

//---------------------------------------------------------------------
//	Include
//---------------------------------------------------------------------
#include <stdio.h>
#include "SysInfoIncludes.h"	//Include any other includes requried for your project (or use a dummy file)

//---------------------------------------------------------------------
//	Generic Defines
//---------------------------------------------------------------------
#define	VSHORT_STRING_LEN		16		//Size of a generic very short string
#define	SHORT_STRING_LEN		64		//Size of a generic short string
#define	LONG_STRING_LEN			256		//Size of a generic long string
#define	VLONG_STRING_LEN		1024	//Size of a generic very long string


//---------------------------------------------------------------------
//	SysInfo DLL API Declarations
//---------------------------------------------------------------------

/** @defgroup admin_group Administrative APIs
 *  @{
 */

/*! 
  \brief	Call this function to retrieve the SysInfo DLL version. This function returns a pointer to constant characters (via use of a static buffer).  Attempting to change the value(s) of constant characters is an error.  No attempt should be made to free the character buffer returned by this function.
  \return	SysInfo DLL version string
*/
SYSINFO_API const wchar_t*	SysInfoDll_GetVersion( void );

/*! 
  \brief		Call this function to begin writing SysInfo log information into a file.
  \param[in]	lpszPath	Directory path to store the log file.
  \return		TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_StartLogging( LPWSTR lpszPath );

/*! 
  \brief		Call this function to begin writing SysInfo log information into an existng file managed by the calling application. 
  \param[in]	fp	File pointer of an opened file to store the log file lines.
  \return		TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL SysInfoDll_StartLoggingExisting(FILE	*fp);

/*! 
  \brief		Call this function to stop writing SysInfo log information.
  \return		TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_StopLogging( void );

/*! 
  \brief		Call this function to retrieve the last error as a string.
  \param[out]	lpszError	Pointer to a string to store the error.
  \param[in]	dwsize		size of the lpszError string buffer.
  \return		TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_GetLastErrorString( LPWSTR lpszError ,
														   DWORD dwsize );

/** @} */ // end of admin_group





/** @defgroup general_group General System Information APIs
 *  @{
 */


/*! 
  \brief			Call this function to return the computer's NetBIOS name as a string.
  \param[out]		lpBuffer	Pointer to a string to store the computer name.
  \param[in,out]	nSize		size of the lpBuffer string buffer. On return, contains the length of the string.
  \return			TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_GetComputerName( LPWSTR lpBuffer ,
													    LPDWORD nSize );

/** @} */ // end of general_group





/** @defgroup os_group Operating System APIs
 *  @{
 */


/*! 
  \brief		Call this function to retrieve the OS version information as a string.
  \note			Use #SysInfoDll_GetOSEx instead to retrieve the OS version info as a struct
  \param[out]	lpszOS		Pointer to a string to store the computer name.
  \param[in]	iLen		size of the lpBuffer string buffer.
  \return		The OS version information as a string if successful, otherwise NULL
*/
SYSINFO_API const wchar_t*	SysInfoDll_GetOS( LPWSTR lpszOS ,
											  int iLen );

/*! 
  \brief		Call this function to retrieve the OS version information.
  \param[out]	lpVersionInformation	Pointer to a structure to store the OS information.
  \return		TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_GetOSEx( LPOSVERSIONINFOW lpVersionInformation );

/** @} */ // end of os_group





/** @defgroup cpu_group CPU APIs
 *  @{
 */

// Defines

/// The maxium number of logical CPUs (including physical cores) that are supported with the CPU test 
#define	MAX_NUM_CPUS			256						
/// The maxium number CPU packages supported
#define	MAX_NUM_CPU_PACKAGES	 32						
///The maxium number of CPU groups (curenly Windows 7 and 2008 support 4 groups of 64 logical CPUs)
#define	MAX_CPU_GROUPS			  4
///The maxium number of CPU in a groups (in 64-bit Windows)
#define	MAX_CPU_IN_GROUP		 64
///The maxium number of CPUs that you can set the CPU affinity for
#define	MAX_AFFINITY_SUPPORT	MAXIMUM_PROC_PER_GROUP	

#define CPU_SPEED_STRING_LENGTH			15
#define CPU_STRING_LEN					34
#define CPUCODE_STRING_LENGTH			16        
#define STEPPING_STRING_LENGTH			6
#define SOCKET_STRING_LENGTH			16        
#define FAB_STRING_LENGTH				5        

enum CPU_OCLK
{
	CPU_OVERCLOCKED_UNKNOWN	= 0,
	CPU_OVERCLOCKED			= 1,
	CPU_UNDERCLOCKED		= 2,
	CPU_OVERCLOCKED_NO		= 3,
	CPU_MAXVAL				= 4,
};

// Structs
typedef struct _CPU_SPECIFICATION
{
    wchar_t		szCPU[CPU_STRING_LEN];
    int			iType;
    int			iFamily;
    int			iModel;
	/// Stepping number from CPU
    int			iStepping;				
    WORD		wCPUCodeName;
	/// index into Stepping string
    WORD		wStepping;
    BYTE		bySocket;												
    BYTE		byFabrication;											
    int			iTJunction;
    wchar_t		szCPUCodeName[CPU_STRING_LEN];
    wchar_t		szStepping[STEPPING_STRING_LENGTH];
    wchar_t		szSocket[SOCKET_STRING_LENGTH];
    wchar_t		szFabrication[FAB_STRING_LENGTH];
} CPU_SPECIFICATION;

typedef struct _CPUMSRINFO 
{
	//GetIA32ArchitecturalMSRInfo
	ULONGLONG	ullMSR0x17;
	ULONGLONG	ullMSR0x1B;
	ULONGLONG	ullMSR0xE7;
	ULONGLONG	ullMSR0xE8;
	ULONGLONG	ullMSR0x19C;
	ULONGLONG	ullMSR0x1A0;

	//GetCore2FamilyMSRInfo_Dynamic
	ULONGLONG	ullMSR0x2A;
	ULONGLONG	ullMSR0x2C;
	ULONGLONG	ullMSR0xCD;

	//Nehalem
	ULONGLONG	ullMSR0xCE;
	ULONGLONG	ullMSR0x1AD;
	ULONGLONG	ullMSR0x1A2;
	ULONGLONG	ullMSR0x1AC;

	int 		iNonIntegerBusRatio;
	/// Current Multiplier 
	float		flBusFrequencyRatio;			
	/// Current Raw frequency of CPUn in MHz
	float		raw_freq_cpu;					

	float		flScalableBusSpeed;
	float		flFSB;

	/// Current base reference clock speed - AMD and Intel Core i7/i5... series
	float		flExternalClock;				

	/// AMD HyperTransport link speed (The value is not valid if set to 0)
	float		flHTFreq;						

	int 		iMinimumRatio;
	int 		iMaximumRatio;

	/// Intel Tubo boost max multiplier when running 1 core
	int 		iMaxTurbo1Core;
	/// Intel Tubo boost max multiplier when running 2 core
	int 		iMaxTurbo2Core;
	/// Intel Tubo boost max multiplier when running 3 core
	int 		iMaxTurbo3Core;
	/// Intel Tubo boost max multiplier when running 4 core
	int 		iMaxTurbo4Core;
	/// The TDP/power limit (where available)
	int			iTDPLimit;
	/// The TDC/current limit (where available)
	int			iTDCLimit;

	int			iTemperatureTarget;
	
	//CPU Turbo
	/// Maximum measured Turbo speed	e.g. 3600MHz 
	float		flCPUSpeedTurbo;	
	/// Maximum theoretical Turbo speed based on the maximum programmed muliplier and the derived base clock speed e.g. 200 x 18 (power state 0 multiplier) = 3600HMz
	float		flCPUSpeedTurboTheoreticalMax;
	/// Programmed maximum Turbo muliplier	e.g. x18
	float		flCPUMultTurbo;					
	///Derived base clock e.g. 200MHz
	float		flCPUExternalClockTurbo;

	//CPU stock settings - Nehalem series
	float		flCPUSpeedStock;				
	float		flCPUMultStock;					
	float		flCPUExternalClockStock;
	
	//CPU TSC CPU speed - generic
	/// TSC measured speed
	float		flCPUTSC;						

	//CPU stock settings - Core 2 series
	float		flScalableBusSpeedStock;
	float		flFSBStock;

	//Overclock settings				
	/// The CPU has been overclocked, underclocked by changing the base CPU clock
	CPU_OCLK	OCLKBaseClock;			
	/// The CPU has been overclocked by increasing the maximum multiplier
	CPU_OCLK	OCLKMultiplier;			
	/// The CPU has been overclocked, method unknown
	CPU_OCLK	OCLKFreq;				

	//AMD Turbo core 
	/// The CPU external clock when the CPU is in Turbo core
	float		flCPUExternalClockBoosted;
	/// The minimum CPU multiplier 
	float		flMinMult;
	/// The maximum CPU multiplier, when not in Turbo core mode 
	float		flMaxMult;
	/// The maximum CPU multiplier when in Turbo core mode 
	float		flBoostedMult;

} CPUMSRINFO;

typedef struct _CPU_SPECIFICATION_STEPPING			
{
	/// Stepping number from CPU
    int     iStepping;							
    wchar_t	szStepping[STEPPING_STRING_LENGTH];
	/// The string to be used for CPUID
	wchar_t	szCPUID[SHORT_STRING_LEN];			
} CPU_SPECIFICATION_STEPPING;

typedef struct _CPU_GROUP_INFO
{
	/// Maximum number of logical CPUs in each CPU group
	DWORD dwMaximumProcessorCount;
	/// Active number of logical CPUs in each CPU group
	DWORD dwActiveProcessorCount;
	/// CPU affinity mask for this group
	KAFFINITY	ActiveProcessorMask;
} CPU_GROUP_INFO;

typedef struct _CPU_GROUPS_INFO
{
	/// Maximum number of CPU groups on the system (for > 64 CPU systems)
	WORD			wMaximumProcessorGroupCount;
	/// Active number of CPU groups on the system (for > 64 CPU systems)
	WORD			wActiveProcessorGroupCount;
	/// Information about each CPU group
	CPU_GROUP_INFO	CPUGroupInfo[MAX_CPU_GROUPS];
} CPU_GROUPS_INFO;

typedef struct _CPUINFO 
{
	/// number of CPU's in the system
	int		number;				
	/// CPU 1 Mhz 
	float	speed1;
	/// CPU 2 Mhz, 0.0 if no 2nd CPU
	float	speed2;
	int		Family;
	int		Model;
	int		stepping;
	/// TRUE or FALSE
	int		MMX;
	/// TRUE or FALSE. TRUE if SSE instructions are available
	int		SIMD;
	/// All CPU features, Spare in V1.0, Features in V1.1
	unsigned int	features;
	/// TRUE if CPU supports ID notification
	int		CPUIDSupport;
	/// Name of Manufacture
	wchar_t	manufacture[20];
	/// 80386, Pentium, etc, Increased from 30bytes to 50 in Bit3
	wchar_t	typestring[50];
	/// Speed string in ASCII 2000 MHz for CPU1. Note: Turbo mode disabled where applicable
	wchar_t	speedstring1[CPU_SPEED_STRING_LENGTH];
	/// Speed string in ASCII 200 MHz, 300 etc for CPU2. Note: Turbo mode disabled where applicable
	wchar_t	speedstring2[CPU_SPEED_STRING_LENGTH];
	/// The number of cache elements. For Intel CPU's only
	int		cacheinfo_num;
	/// Cache size. 128, 512, 1024, etc.., -1 = Unknown. For all CPU's
	int		L2_cache_size;
	/// only for PIII type 8 and above. 0=Not supported, 1 = Celeron, 2 = PIII, 3= PIII Xeon
	int		Brand;
    /// TRUE if serial number is available and not disabled. FALSE otherwise
	int		SerialAvail;
	/// Part 1 of the 96 bit serial number
	DWORD	SerialNum1;
	/// Part 2 of the 96 bit serial number
	DWORD	SerialNum2;
	/// Part 3 of the 96 bit serial number
	DWORD	SerialNum3;
	/// TRUE or FALSE. TRUE if SSE2 instructions are available
	int		SMID_SSE2;
	/// TRUE or FALSE. TRUE if 3DNow instructions are available
	int		AMD3DNow;
	/// The number of logical CPUs - may be larger than physical with hyperthreading
	int		iLogicalsPerCore;
	/// Whether Hyperthreading is enabled, supported, etc
	int		iHyperThreadStatus;
	/// Cache size in KB. 128, 512, 1024, etc.., -1 = Unknown 
	int		L3_cache_size;
	/// TRUE or FALSE. TRUE if SSE3 instructions are available
	int		SMID_SSE3;
	/// Number of cores per CPU package
	int		iCoresPerPackage;
	/// The maximum CPUID basic information level supported
	int		MaxBasicInputValue;
	/// Physical Address Extension is not supported, 1 PAE is supported
	int		PAE;
	/// DEP/XD is not supported, 1 DEP/XD is supported
	int		DEP;				
	/// SSE4.1 TRUE/FALSE
	int		SMID_SSE4_1;						
	/// SSE4.2 TRUE/FALSE
	int		SMID_SSE4_2;						
	/// EMT64 TRUE/FALSE
	int		EMT64;								
	/// L1 instruction cache size
	int		L1_instruction_cache_size;			
	/// Trace cache size
	int		Trace_cache_size;					
	/// L1 data cache size
	int		L1_data_cache_size;					
	/// CPU prefetch size
	int		Prefetching;						
	/// Num. L1 data caches per CPU package
	int		L1_data_caches_per_package;			
	/// Num. L1 instruction caches per CPU package
	int		L1_instruction_caches_per_package;	
	/// Num. L2 data caches per CPU package
	int		L2_caches_per_package;				
	/// Num. L3 data caches per CPU package
	int		L3_caches_per_package;				
	CPUMSRINFO MSR[MAX_NUM_CPUS];
	/// TRUE or FALSE. TRUE if SSSE3 instructions are available
	int		SMID_SSSE3;							
	/// TRUE or FALSE. TRUE if SSE4a instructions are available
	int		SMID_SSE4a;							
	/// TRUE or FALSE. TRUE if processor supports Intel Virtulization Technology
	int		VMX;								
	/// TRUE or FALSE. TRUE if processor supports Intel Trusted Execution Technology
	int		SMX;								
	/// Themal monitor supported (via MSRs)
	int		ACPI;								
	/// The string to be used for EMT64
	wchar_t	szEMT[VSHORT_STRING_LEN];			
	int		iManufacture;						//CPU_manufacturer - without the string compare
	/// Hyperthreading capable
	int		HTT	;								
	/// CPU specification
	CPU_SPECIFICATION Spec;						
	/// CPU stepping for each CPU package
	CPU_SPECIFICATION_STEPPING CPUPackage[MAX_NUM_CPU_PACKAGES] ;	
	/// Digital temperature sensor supported		
	bool	bDTS;								
	/// Intel Tubo boost supported
	bool	bIntelTurboBoost;					
	/// Intel Tubo boost max multiplier when running 1 core
	int		iMaxTurbo1Core;						
	/// Intel Tubo boost max multiplier when running 2 core
	int		iMaxTurbo2Core;						
	/// Intel Tubo boost max multiplier when running 3 core
	int		iMaxTurbo3Core;						
	/// Intel Tubo boost max multiplier when running 4 core
	int		iMaxTurbo4Core;						
	/// Maximum measured Turbo speed	e.g. 3600MHz 
	float	flCPUSpeedTurbo;					
	/// Maximum theoretical Turbo speed based on the maximum programmed muliplier and the derived base clock speed e.g. 200 x 18 (power state 0 multiplier) = 3600HMz
	float	flCPUSpeedTurboTheoreticalMax;					
	/// AES fucntions supported
	int		AES;

	//Overclock settings						
	/// The CPU has been overclocked, underclocked by changing the base CPU clock
	CPU_OCLK	OCLKBaseClock;					
	/// The CPU has been overclocked by increasing the maximum multiplier
	CPU_OCLK	OCLKMultiplier;					
	/// The CPU has been overclocked, method unknown
	CPU_OCLK	OCLKFreq;						
	/// The CPU support AMD Turbo Core (CPB - Core Performacne Boost)
	bool	bAMDTurboCore;	
	/// Whether Hyperthreading is enabled, supported, etc as a string
	wchar_t	szHyperThreadStatus[LONG_STRING_LEN]; 
	/// A summary of the CPUs on the system, as a string
	wchar_t	szCPUSummary[LONG_STRING_LEN]; 

	/// Information about all of the CPU groups (used for setting CPU affinity for systems with more than 1 CPU group e.g. > 64 CPUs) 
	CPU_GROUPS_INFO CPUGroupsInfo;

	////Advanced Vector Extensions intructions supported
	int		AVX;								
	
	/// Reserved for future expansion
	int		Future_expansion9[99];
} CPUINFO;

/*! 
  \brief Return code for #SysInfoDll_GetCPUInfo
*/
enum CPUINFO_STATUS
{
	CPUINFO_SUCCESS			= 0,
	CPUINFO_ACCESS_DENIED	= 1,
	CPUINFO_INIT_DIRECTIO	= 2,
	CPUINFO_DIRECTIO_VERSION = 3,
	CPUINFO_NOT_ALL_CPUS_AVAILABLE = 4,
};

/*! 
  \brief Call this function to return the CPU information structure.
  \param[out] pCPUinfo	Pointer to a structure to store the CPU information.
  \return 0 if successful, otherwise an error code (see #CPUINFO_STATUS)
*/
SYSINFO_API int				SysInfoDll_GetCPUInfo( CPUINFO	*pCPUinfo );


/*! 
  \brief Call this function to return the CPU information structure (with reduced low level infomration).
  \param[out] pCPUinfo	Pointer to a structure to store the CPU information.
  \return 0 if successful, otherwise an error code (see #CPUINFO_STATUS)
*/
SYSINFO_API int				SysInfoDll_GetCPUInfoMin( CPUINFO	*pCPUinfo );

/** @} */ // end of cpu_group





/** @defgroup mem_group Memory APIs
 *  @{
 */

#define MODULE_PARTNO_LEN	18
#define MAX_MEMORY_SLOTS	16

/*! 
  \brief Return code for #SysInfoDll_GetMemSPDInfo
*/
enum SPDINFO_STATUS
{
	SPDINFO_SUCCESS			= 0,
	SPDINFO_ACCESS_DENIED	= 1,
	SPDINFO_INIT_DIRECTIO	= 2,
	SPDINFO_DIRECTIO_VERSION = 3,
	SPDINFO_ACCESS_COLLISION = 4,
};

/*--- SPDINFO.type ---*/
#define SPDINFO_MEMTYPE_SDRAM		0x04
#define SPDINFO_MEMTYPE_DDR			0x07
#define SPDINFO_MEMTYPE_DDR2		0x08
#define SPDINFO_MEMTYPE_DDR2FB		0x09
#define SPDINFO_MEMTYPE_DDR2FBPROBE	0x0A
#define SPDINFO_MEMTYPE_DDR3		0x0B

/*--- SPDINFO.DDR2SDRAM.EPP.profileType ---*/
#define SPDINFO_DDR2_EPP_PROFILE_ABBR	0xA1
#define SPDINFO_DDR2_EPP_PROFILE_FULL	0XB1

typedef struct _SPDINFO 
{
	/// String summaries used in the formatted reports
	wchar_t szHeading[SHORT_STRING_LEN];
	/// Line 1 of the report memory summary
	wchar_t szLine1[SHORT_STRING_LEN];
	/// Line 2 of the report memory summary
	wchar_t szLine2[SHORT_STRING_LEN];
	/// Line 3 of the report memory summary
	wchar_t szLine3[SHORT_STRING_LEN];

	/// Type of RAM. See #define \link #SPDINFO_MEMTYPE_SDRAM SPDINFO_MEMTYPE_* \endlink
	int type;
	/// SPD revision
	int spdRev; 
	/// TRUE for registered
	bool registered; 
	/// TRUE for ECC
	bool ecc;	
	/// Year of manufacture (only valid if > 0)
	int year;	
	/// Week of manufacture (only valid if > 0)
	int week;	
	/// DIMM slot number
	int dimmNum;	
	/// Jedec ID code 
	UINT64 jedecID; 
	/// Jedec manufacture name
	wchar_t jedecManuf[SHORT_STRING_LEN];
	
	/// Module part number
	char modulePartNo[MODULE_PARTNO_LEN];
	/// Module revision
	int moduleRev;
	/// Module serial number
	int moduleSerialNo;
	/// Module manufacturing location
	int moduleManufLoc;

	/// Number of row addressing bits
	int rowAddrBits;
	/// Number of column addressing bits
	int colAddrBits;
	/// Number of banks
	int numBanks;
	/// Number of ranks
	int numRanks;
	/// Device width in bits
	int deviceWidth;
	/// Bus width in bits
	int busWidth;
	/// Module voltage
	wchar_t moduleVoltage[SHORT_STRING_LEN];

	/// Clock speed in MHz
	float clkspeed;
	/// Module speed
	wchar_t szModulespeed[SHORT_STRING_LEN];
	/// Memory capacity in MB
	int size;

	/// CAS latencies supported
	wchar_t CASSupported[SHORT_STRING_LEN];
	/// Minimum clock cycle time in ns
	float tCK;
	/// Minimum CAS latency time in ns
	float tAA;
	/// Minimum RAS to CAS delay in ns
	float tRCD;
	/// Minimum Row Precharge time in ns
	float tRP;
	/// Minimum Active to Precharge Time in ns
	float tRAS;
	/// Minimum Row Active to Row Active delay in ns
	float tRRD;
	/// Minimum Auto-Refresh to Active/Auto-Refresh Time in ns
	float tRC;
	/// Minimum Auto-Refresh to Active/Auto-Refresh Command Period in ns
	float tRFC;
	
	/// RAM-type specific attributes
	union SPECIFICINFO {
		/// SDRAM-specific attributes
		struct SDRSDRAMINFO {
			/// Data access time from clock
			float tAC;
			/// Clock cycle time at medium CAS latency in ns
			float tCKmed;
			/// Data access time at medium CAS latency in ns
			float tACmed;
			/// Clock cycle time at short CAS latency in ns
			float tCKshort;
			/// Data access time at short CAS latency in ns
			float tACshort;
			
			/// Address/Command Setup Time before clock in ns
			float tIS;
			/// Address/Command Hold Time after clock in ns
			float tIH;
			/// Data Input Setup Time before strobe in ns
			float tDS;
			/// Data Input Hold Time after strobe in ns
			float tDH;

			/// CS latencies supported
			wchar_t CSSupported[SHORT_STRING_LEN];
			/// WE latencies supported
			wchar_t WESupported[SHORT_STRING_LEN];

			/// Burst Lengths supported
			wchar_t BurstLengthsSupported[SHORT_STRING_LEN];
			/// Refresh rate
			wchar_t RefreshRate[SHORT_STRING_LEN];

			/// TRUE for buffered address/control inputs
			bool buffered;
			/// TRUE for on-card PLL
			bool OnCardPLL;
			/// TRUE for buffered DQMB inputs
			bool BufferedDQMB;
			/// TRUE for registerd DQMB inputs
			bool RegisteredDQMB;
			/// TRUE for differential clock input
			bool DiffClockInput;
	
			/// TRUE for Early RAS# Precharge supported
			bool EarlyRASPrechargeSupported;
			/// TRUE for Auto-Precharge supported
			bool AutoPrechargeSupported;
			/// TRUE for Precharge all supported
			bool PrechargeAllSupported;
			/// TRUE for write/read burst supported
			bool WriteReadBurstSupported;

		} SDRSDRAM;
		/// DDR-specific attributes
		struct DDR1SDRAMINFO {
			/// Data access time from clock
			float tAC;
			/// Clock cycle time at medium CAS latency in ns
			float tCKmed;
			/// Data access time at medium CAS latency in ns
			float tACmed;
			/// Clock cycle time at short CAS latency in ns
			float tCKshort;
			/// Data access time at short CAS latency in ns
			float tACshort;
			/// Maximum clock cycle time in ns
			float tCKmax;

			/// Address/Command Setup Time before clock in ns
			float tIS;
			/// Address/Command Hold Time after clock in ns
			float tIH;
			/// Data Input Setup Time before strobe in ns
			float tDS;
			/// Data Input Hold Time after strobe in ns
			float tDH;

			/// Maximum skew between DQS and DQ signals in ns
			float tDQSQ;
			/// Maximum Read Data Hold Skew Factor in ns
			float tQHS;

			/// CS latencies supported
			wchar_t CSSupported[SHORT_STRING_LEN];
			/// WE latencies supported
			wchar_t WESupported[SHORT_STRING_LEN];

			/// Burst Lengths supported
			wchar_t BurstLengthsSupported[SHORT_STRING_LEN];
			/// Refresh rate
			wchar_t RefreshRate[SHORT_STRING_LEN];

			/// TRUE for buffered address/control inputs
			bool buffered;
			/// TRUE for on-card PLL
			bool OnCardPLL;
			/// TRUE for FET Switch On-Card Enable
			bool FETOnCardEnable;
			/// TRUE for FET Switch External Enable
			bool FETExtEnable;
			/// TRUE for Differential Clock Input
			bool DiffClockInput;

			/// TRUE for Weak Driver included
			bool WeakDriverIncluded;
			/// TRUE for Concurrent Auto Precharge supported
			bool ConcAutoPrecharge;
			/// TRUE for Fast AP supported
			bool FastAPSupported;

			/// Module bank density
			wchar_t bankDensity[VSHORT_STRING_LEN];
			/// Module height
			wchar_t moduleHeight[VSHORT_STRING_LEN];
		} DDR1SDRAM;
		/// DDR2-specific attributes
		struct DDR2SDRAMINFO {
			/// Data access time from clock
			float tAC;
			/// Clock cycle time at medium CAS latency in ns
			float tCKmed;
			/// Data access time at medium CAS latency in ns
			float tACmed;
			/// Clock cycle time at short CAS latency in ns
			float tCKshort;
			/// Data access time at short CAS latency in ns
			float tACshort;
			/// Maximum clock cycle time in ns
			float tCKmax;

			/// Write recovery time in ns
			float tWR;
			/// Internal write to read command delay in ns
			float tWTR;
			/// Internal read to precharge command delay in ns
			float tRTP;

			/// Address/Command Setup Time before clock in ns
			float tIS;
			/// Address/Command Hold Time after clock in ns
			float tIH;
			/// Data Input Setup Time before strobe in ns
			float tDS;
			/// Data Input Hold Time after strobe in ns
			float tDH;

			/// Maximum skew between DQS and DQ signals in ns
			float tDQSQ;
			/// Maximum Read Data Hold Skew Factor in ns
			float tQHS;
			/// PLL Relock Time in ns
			float tPLLRelock;

			/// DRAM Package type
			wchar_t DRAMPackage[VSHORT_STRING_LEN];

			/// Burst Lengths supported
			wchar_t BurstLengthsSupported[SHORT_STRING_LEN];
			/// Refresh rate
			wchar_t RefreshRate[SHORT_STRING_LEN];

			/// Number of PLLs on DIMM
			int numPLLs;
			/// TRUE for FET Switch External Enable
			bool FETExtEnable;
			/// TRUE for Analysis probe installed
			bool AnalysisProbeInstalled;
			/// TRUE for Weak Driver supported
			bool WeakDriverSupported;
			/// TRUE for 50 ohm ODT supported
			bool _50ohmODTSupported;
			/// TRUE for Partial Array Self Refresh supported
			bool PASRSupported;

			/// Module type
			wchar_t moduleType[SHORT_STRING_LEN];
			/// Module height
			wchar_t moduleHeight[VSHORT_STRING_LEN];

			/// TRUE if Enhanced Performance Profiles (EPP) supported
			bool EPPSupported;

			/// EPP-specific attributes
			struct EPPInfo {
				/// EPP Profile Type. See #define \link #SPDINFO_DDR2_EPP_PROFILE_ABBR SPDINFO_MEMTYPE_* \endlink
				int profileType;
				/// EPP Profile that should be loaded to maximize system performance
				int optimalProfile;

				/// EPP Profile attributes
				union PROFILEINFO {
					/// EPP Abbreviated Profile attributes
					struct ABBRPROFILEINFO {
						/// TRUE if this profile contains valid data
						bool enabled;
						/// The voltage level required for this profile
						wchar_t voltageLevel[SHORT_STRING_LEN];
						/// Clock speed in MHz
						float clkspeed;
						/// Address command rate. 1T - commands can be sent on every clock edge. 2T - commands can be sent on every other clock edge.
						int cmdRate;

						/// CAS latency supported
						int CASSupported;
						/// Minimum clock cycle time in ns
						float tCK;

						/// Minimum RAS to CAS delay in ns
						float tRCD;
						/// Minimum Row Precharge time in ns
						float tRP;
						/// Minimum Active to Precharge Time in ns
						float tRAS;

					} abbrProfile[4];
					struct FULLPROFILEINFO {
						/// TRUE if this profile contains valid data
						bool enabled;
						/// Voltage level required for this profile
						wchar_t voltageLevel[SHORT_STRING_LEN];
						/// Clock speed in MHz
						float clkspeed;
						/// Address command rate. 1T - commands can be sent on every clock edge. 2T - commands can be sent on every other clock edge.
						int cmdRate;

						/// Address Drive Strength
						wchar_t addrDriveStrength[VSHORT_STRING_LEN];
						/// Chip Select Drive Strength
						wchar_t CSDriveStrength[VSHORT_STRING_LEN];
						/// Clock Drive Strength
						wchar_t clockDriveStrength[VSHORT_STRING_LEN];
						/// Data Drive Strength
						wchar_t dataDriveStrength[VSHORT_STRING_LEN];
						/// DQS Drive Strength
						wchar_t DQSDriveStrength[VSHORT_STRING_LEN];
						/// Address/Command Fine Delay
						wchar_t addrCmdFineDelay[VSHORT_STRING_LEN];
						/// Address/Command Setup Time
						wchar_t addrCmdSetupTime[VSHORT_STRING_LEN];
						/// Chip Select Fine Delay
						wchar_t CSFineDelay[VSHORT_STRING_LEN];
						/// Chip Select Setup Time
						wchar_t CSSetupTime[VSHORT_STRING_LEN];

						/// CAS latency supported
						int CASSupported;
						/// Minimum clock cycle time in ns
						float tCK;

						/// Minimum RAS to CAS delay in ns
						float tRCD;
						/// Minimum Row Precharge time in ns
						float tRP;
						/// Minimum Active to Precharge Time in ns
						float tRAS;

						/// Write recovery time in ns
						float tWR;
						/// Minimum Auto-Refresh to Active/Auto-Refresh Time in ns
						float tRC;

					} fullProfile[2];
				} profileData;				
			} EPP;
			
		} DDR2SDRAM;
		/// DDR2FB-specific attributes
		struct DDR2FBSDRAMINFO {
			/// Maximum clock cycle time in ns
			float tCKmax;
			/// Write recovery time in ns
			float tWR;
			/// Internal write to read command delay in ns
			float tWTR;
			/// Internal Read to Precharge Command Delay in ns
			float tRTP;
			/// Minimum Four Activate Window Delay in ns
			float tFAW;

			/// Write Recovery Times Supported
			wchar_t WRSupported[SHORT_STRING_LEN];
			/// Write Latencies Supported
			wchar_t WESupported[SHORT_STRING_LEN];
			/// Additive Latencies Supported
			wchar_t ALSupported[SHORT_STRING_LEN];

			/// Burst Lengths supported
			wchar_t BurstLengthsSupported[SHORT_STRING_LEN];
			/// Refresh rate
			wchar_t RefreshRate[SHORT_STRING_LEN];
			/// Terminations Supported
			wchar_t TerminationsSupported[SHORT_STRING_LEN];
			/// Drivers Supported
			wchar_t DriversSupported[SHORT_STRING_LEN];

			/// Module type
			wchar_t moduleType[SHORT_STRING_LEN];
			/// Module height
			wchar_t moduleHeight[VSHORT_STRING_LEN];
			/// Module thickness
			wchar_t moduleThickness[VSHORT_STRING_LEN];

			/// DRAM manufacture ID
			int DRAMManufID;
		} DDR2FBSDRAM;
		/// DDR3-specific attributes
		struct DDR3SDRAMINFO {
			/// Write recovery time in ns
			float tWR;
			/// Internal write to read command delay in ns
			float tWTR;
			/// Internal Read to Precharge Command Delay in ns
			float tRTP;
			/// Minimum Four Activate Window Delay in ns
			float tFAW;

			/// TRUE for RZQ / 6 supported
			bool RZQ6Supported;
			/// TRUE for RZQ / 7 supported
			bool RZQ7Supported;
			/// TRUE for DLL-Off Mode supported
			bool DLLOffModeSupported;
			/// Maximum operating temperature
			int OperatingTempRange;
			/// Refresh rate at extended operating temperature range
			int RefreshRateExtTempRange;
			/// TRUE forAuto self refresh supported
			bool autoSelfRefresh;
			/// TRUE forOn-die thermal sensor readout supported
			bool onDieThermalSensorReadout;
			/// TRUE forPartial Array Self Refresh supported
			bool partialArraySelfRefresh;
			/// TRUE for thermal sensor present
			bool thermalSensorPresent;

			/// Non-standard SDRAM type
			wchar_t nonStdSDRAMType[SHORT_STRING_LEN];

			/// Module type
			wchar_t moduleType[SHORT_STRING_LEN];
			/// Module height
			int moduleHeight;
			/// Module thickness (front)
			int moduleThicknessFront;
			/// Module thickness (back)
			int moduleThicknessBack;
			/// Module width
			wchar_t moduleWidth[VSHORT_STRING_LEN];

			/// Reference raw card used
			wchar_t moduleRefCard[SHORT_STRING_LEN];
			/// DRAM manufacture ID
			int DRAMManufID;
			/// Number of DRAM rows
			int numDRAMRows;
			/// Number of registers
			int numRegisters;
			/// Register manufacturer
			char regManuf;
			/// Register type
			char regDeviceType;
			/// Register revision
			int regRev;

			/// TRUE if Intel Extreme Memory Profile (XMP) supported
			bool XMPSupported;

			/// XMP-specific attributes
			struct XMPInfo {
				/// Intel Extreme Memory Profile Revision
				int revision;

				/// XMP profile attributes
				struct XMPProfile {
					/// TRUE if this profile contains valid data
					bool enabled;

					/// Recommended number of DIMMs per channel
					int dimmsPerChannel;

					/// Module VDD Voltage Level
					wchar_t moduleVdd[SHORT_STRING_LEN];
					/// Clock speed in MHz
					float clkspeed;

					/// CAS latencies supported
					wchar_t CASSupported[SHORT_STRING_LEN];
					/// Minimum clock cycle time in ns
					float tCK;
					/// Minimum CAS latency time in ns
					float tAA;
					/// Minimum RAS to CAS delay in ns
					float tRCD;
					/// Minimum Row Precharge time in ns
					float tRP;
					/// Minimum Active to Precharge Time in ns
					float tRAS;
					/// Minimum Row Active to Row Active delay in ns
					float tRRD;
					/// Minimum Auto-Refresh to Active/Auto-Refresh Time in ns
					float tRC;
					/// Minimum Auto-Refresh to Active/Auto-Refresh Command Period in ns
					float tRFC;

					/// Write recovery time in ns
					float tWR;
					/// Internal write to read command delay in ns
					float tWTR;
					/// Internal Read to Precharge Command Delay in ns
					float tRTP;
					/// Minimum Four Activate Window Delay in ns
					float tFAW;

					/// Minimum CAS Write Latency Time in ns
					float tCWL;
					/// Maximum REFI (Average Periodic Refresh Interval) in us
					float tREFI;

					/// Write to Read CMD Turn-around Time Optimizations
					wchar_t WRtoRDTurnaround[SHORT_STRING_LEN];

					/// Read to Write CMD Turn-around Time Optimizations
					wchar_t RDtoWRTurnaround[SHORT_STRING_LEN];

					/// Back to Back CMD Turn-around Time Optimizations
					wchar_t back2BackTurnaround[SHORT_STRING_LEN];

					/// System CMD Rate Mode
					int cmdRateMode;
					
				} profile[2];
			} XMP;
		} DDR3SDRAM;
	} specific;

} SPDINFO;

typedef struct _MEMMODULEINFO
{
	/// number of memory modules
	int						iNumMemSticks;
	/// SPD data for each memory module
	SPDINFO					memSPDInfo[MAX_MEMORY_SLOTS];
} MEMMODULEINFO;

/*! 
  \brief Call this function to return the Memory information structure.
  \param[out] pMemInfoEx	Pointer to a structure to store the Memory information.
  \return TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_GetMemoryInfo( MEMORYSTATUSEX *pMemInfoEx );

/*! 
  \brief Call this function to retrieve the SPD information from all memory modules.
  \param[out] pMemModuleInfo	Pointer to a structure to store the Memory SPD information.
  \return 0 if successful, otherwise an error code (see #SPDINFO_STATUS)
*/
SYSINFO_API int				SysInfoDll_GetMemSPDInfo( MEMMODULEINFO *pMemModuleInfo );

/** @} */ // end of mem_group




/* Display APIs */

/** @defgroup display_group Display APIs 
 *  @{
 */
// defines
///Max number of monitors supported
#define NUMMONITORS					4						
#define MAX_MONITOR_INFO_LEN		256

#define MAX_DISPLAY_DEVICES			16						//Max number of "DISPLAY<n>" devices - more than the number of adapters, adapters will be started for each DISPLAY device
/// Max number of monitors per adapter
#define MAX_MONITORS				4						
#define MAX_STRINGLEN				128	
#define MAX_DEV_MODES				800
#define MAX_VIDEO_INFO_LEN			128

// structs
typedef struct _MONITOR_ATTRIBUTES
{
	/// Color depth - Bits per pixel
	int				iBPP;									
	/// Resolution - X
	int				iX;										
	/// Resolution - Y
	int				iY;										
	/// Refresh rate
	float			flRefreshRate;							
	/// Monitor Identifier (as in "Identify monitor")
	wchar_t			szMonitorNumber[MAX_MONITOR_INFO_LEN];	
	/// A Monitor Attribute display string
	wchar_t			szDisplay[MAX_MONITOR_INFO_LEN];		
} MONITOR_ATTRIBUTES;

typedef struct _MULTIMONITOR_DYN
{
	int					iNumberMonitorsInit;					
	int					iNumberMonitors;					
	HMONITOR			hMon[NUMMONITORS];
	MONITORINFOEX		mi[NUMMONITORS];
	MONITOR_ATTRIBUTES	ma[NUMMONITORS];
} MULTIMONITOR_DYN;

typedef struct _SYSINFO_DISPLAY_DEVICE
{
	bool	bDisplay;									//This DISPLAY<n> device is the first instance of a particular adapter (e.g. DISPLAY1 and DISPLAY3 may be on the one Graphics card, When displaying adapters, only display DISPLAY1)
	wchar_t	szDisplayDevice[MAX_VIDEO_INFO_LEN];
	wchar_t	szDeviceDescription[MAX_VIDEO_INFO_LEN];
	wchar_t	szDeviceID[MAX_VIDEO_INFO_LEN];
	wchar_t	szDeviceKey[MAX_VIDEO_INFO_LEN];
	
	bool	bDriverInstalled;
	wchar_t	szDeviceDescription2[MAX_VIDEO_INFO_LEN];
	wchar_t	szChipType[MAX_VIDEO_INFO_LEN];
	wchar_t	szDACType[MAX_VIDEO_INFO_LEN];
	wchar_t	szBIOSString[MAX_VIDEO_INFO_LEN];
	wchar_t	szInstalledDisplayDrivers[MAX_VIDEO_INFO_LEN];
	DWORD	dwMemorySize;

	wchar_t	szDriverProviderName[MAX_VIDEO_INFO_LEN];
	wchar_t	szDriverVersion[MAX_VIDEO_INFO_LEN];
	wchar_t	szDriverDate[MAX_VIDEO_INFO_LEN];

	int		iNumMonitors;
	/// Index into MULTIMONITOR_DYN for monitor
	int		iMonitorIndex[MAX_MONITORS];				
} SYSINFO_DISPLAY_DEVICE;

typedef struct _DISPLAY_DEVICES
{
	/// number of display devices, DISPLAY<n>
	int						iNumDD;								//number of display devices, DISPLAY<n>
	/// Display devices
	SYSINFO_DISPLAY_DEVICE	dd[MAX_DISPLAY_DEVICES];			//Display devices
} DISPLAY_DEVICES;


/*! 
  \brief		Call this function to retrieve the monitor and display devices information, as well as a summary string.
  \param[out]	MM			Pointer to a structure to store the monitor information.
  \param[out]	DD			Pointer to a structure to store the display device information.
  \param[out]	szSummary	Pointer to a string to store a summary of the monitor and display information.
  \param[in]	iLen		Size of the szSummary string buffer.
  \return		TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_GetGraphicsInfo( MULTIMONITOR_DYN *MM ,
													    DISPLAY_DEVICES *DD , 
													    LPWSTR szSummary , 
													    int iLen );


/*! 
  \brief		Call this function to retrieve a summary string for the system graphics hardware.
  \param[out]	szSummary	Pointer to a string to store a summary of the monitor and display information.
  \param[in]	iLen		Size of the szSummary string buffer.
  \return		summary string
*/
SYSINFO_API wchar_t * SysInfoDll_GetGraphicsSummaryLine(wchar_t * szSummary, int iLen);

/** @} */ // end of display_group




/** @defgroup usb_group USB Controller and Device APIs
 *  @{
 */

// defines
#define MAXUSBHOSTCONTROLLERS				10
/// Max # of USB devices per host controller
#define MAXUSBDEVICES						14					//Per host controller
//#define USBSTRINGLEN						128
#define USBSTRINGLEN						100					//Reduced the maximum string length - SI101017

// structs
typedef struct _USB_HOST_CONTROLLER
{
	/// Number of USB devices connecter to this Host Controller
	int	iNumDev;
	/// Unique Host controller device name
	wchar_t szDriverKeyName[USBSTRINGLEN];						
	/// Host controller name 
	wchar_t szHostController[USBSTRINGLEN];						
	/// USB Device names
	wchar_t szDevice[MAXUSBDEVICES][USBSTRINGLEN];				
} USB_HOST_CONTROLLER;

typedef struct _USB_HOST_CONTROLLERS
{
	/// Number of USB Host controllers
	int	iNumHostControllers;									
	USB_HOST_CONTROLLER USBHostController[MAXUSBHOSTCONTROLLERS];
} USB_HOST_CONTROLLERS;

/*! 
  \brief		Call this function to retrieve the USB controller and device information.
  \param[out]	USBHostControllers	Pointer to a structure to store the USB controller information
  \return		TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_GetUSBInfo(USB_HOST_CONTROLLERS *USBHostControllers);

/** @} */ // end of usb_group



/** @defgroup wmi_group Windows Management Instrumentation (WMI) APIs
 *  @{
 */
// defines
#define WMISI_VSHORTSTRING				25	
#define WMISI_SHORTSTRING				50	
#define WMISI_INFO_VSHORTSTRING			100	
#define WMISI_INFO_SHORTSTRING			200	

#define MAXBIOSMODULES					1
#define MAXBIOSCHARACTERISTICS			100

/// Max number of CD/DVD drives
#define	MAXNUMCDDVD						20		
/// Max number of page files
#define WMISI_MAXPAGEFILES				 2		
#define MAX_FILENAME					512
/// Max number of disks that can be tested at the same time	
#define	MAXNUMDISKS						80		
#define MAXLOGICALDISKS					26
#define MAXRAMMODULES					64	
#define MAXCOMPUTERSYSTEMS				1
#define MAXBASEBOARDS					1
#define MAXSERIALPORTS					64
#define MAXSPARALLELPORTS				10
#define MAXNETWORKADAPTERS				20
#define MAXPORTCONNECTORS				50

// structs
typedef struct _BIOSINFO
{
	wchar_t	szBIOS_Manufacturer[MAXBIOSMODULES][WMISI_INFO_SHORTSTRING];		
	wchar_t	szBIOS_ReleaseDate[MAXBIOSMODULES][WMISI_INFO_SHORTSTRING];			
	wchar_t	szBIOS_BIOSVersion[MAXBIOSMODULES][WMISI_INFO_SHORTSTRING];			
} BIOSINFO;

typedef struct _VIRTUALMEMORYINFO
{
	/// Number of page files
	int		NumberPageFiles;									
	/// Page file names
	wchar_t	PageFileName[WMISI_MAXPAGEFILES][MAX_FILENAME];		
	DWORD	dwPageFileAllocatedBaseSize[WMISI_MAXPAGEFILES];		
} VIRTUALMEMORYINFO;

typedef struct _OPTICALDRIVEINFO
{
	wchar_t OpticalDriveSummary[LONG_STRING_LEN];
	wchar_t	OpticalDriveModel[MAXNUMCDDVD][WMISI_SHORTSTRING];
	int		OpticalDrivesNum;
} OPTICALDRIVEINFO;

typedef struct _PHYSDISKINFO
{
	DWORD	dwDisk_Index;				//Timr - This is the disk index that matches up with the samrt data / disk management order					
	UINT64	Size;
	bool	bFixed;
	bool	bSSD;
	bool	bHotPlug;
	wchar_t	Name[WMISI_SHORTSTRING];
	wchar_t	szDisk_Description[WMISI_SHORTSTRING];		
	wchar_t	szSummary[WMISI_INFO_SHORTSTRING];
	wchar_t	szSerial[WMISI_INFO_SHORTSTRING];	//Timr added
} PHYSDISKINFO;

typedef struct _PHYSDISKINFOSUMMARY
{
	UINT64	Size;
	int		iNum;
	bool	bSSD;

} PHYSDISKINFOSUMMARY;

typedef struct _PHYSICALDRIVEINFO
{
	int				iNumPhysDisks;
	wchar_t			tszHDDSummary[LONG_STRING_LEN];
	PHYSDISKINFO	PhysDisk[MAXNUMDISKS];
} PHYSICALDRIVEINFO;

typedef struct _DISKVOLUMEINFO
{
	int		iNumLogicalDisks;
	wchar_t	szLogicalDisk_VolumeName[MAXLOGICALDISKS][WMISI_SHORTSTRING];	
	wchar_t	szLogicalDisk_Name[MAXLOGICALDISKS][WMISI_SHORTSTRING];		
	wchar_t	szLogicalDisk_Description[MAXLOGICALDISKS][WMISI_SHORTSTRING];	
	wchar_t	szLogicalDisk_FileSystem[MAXLOGICALDISKS][WMISI_SHORTSTRING];	
	wchar_t	szLogicalDisk_ProductName[MAXLOGICALDISKS][WMISI_SHORTSTRING];	
	wchar_t	szLogicalDisk_VolumeSummary[MAXLOGICALDISKS][LONG_STRING_LEN];	
	wchar_t	szLogicalDisk_Serial[MAXLOGICALDISKS][LONG_STRING_LEN];	
	int		iLogicalDisk_DriveType[MAXLOGICALDISKS];								
	ULONGLONG ullLogicalDisk_Totalbytes[MAXLOGICALDISKS];
	ULONGLONG ullLogicalDisk_Freebytes[MAXLOGICALDISKS];
} DISKVOLUMEINFO;


/*
Value Meaning 
0 - Unknown
1 - Other
2 - DRAM
3 - Synchronous DRAM
4 - Cache DRAM
5 - EDO
6 - EDRAM
7 - VRAM
8 - SRAM
9 - RAM
10 - ROM
11 - Flash
12 - EEPROM
13 - FEPROM
14 - EPROM
15 - CDRAM
16 - 3DRAM
17 - SDRAM
18 - SGRAM
19 - RDRAM
20 - DDR
21 - DDR-2
*/
enum RAMTYPE
{
	RAMTYPE_UNDEFINED = 0,		//0
	RAMTYPE_OTHER,
	RAMTYPE_UNKNOWN,			
	RAMTYPE_DRAM,
	RAMTYPE_EDRAM,
	RAMTYPE_VRAM,
	RAMTYPE_SRAM,
	RAMTYPE_RAM,
	RAMTYPE_ROM,
	RAMTYPE_FLASH,
	RAMTYPE_EEPROM,				//10
	RAMTYPE_FEPROM,				
	RAMTYPE_EPROM,
	RAMTYPE_CDRAM,
	RAMTYPE_3DRAM,
	RAMTYPE_SDRAM,				//15
	RAMTYPE_SGRAM,				
	RAMTYPE_RDRAM,
	RAMTYPE_DDR,
	RAMTYPE_DDR2,
	RAMTYPE_DDR2_FB_DIMM,		//20
	RAMTYPE_DDR3,				//21	
	RAMTYPE_TERMINATOR,			//
};

typedef struct _RAMINFO_WMI
{
	DWORD		dwRAM_MemoryType[MAXRAMMODULES];	
	ULONGLONG	ullWMI_RAM_Capacity[MAXRAMMODULES];
	DWORD		dwWMI_RAM_MemoryType[MAXRAMMODULES];
} RAMINFO_WMI;

typedef struct _COMPUTERMODELINFO
{
	wchar_t	szComputer_Model[MAXCOMPUTERSYSTEMS][WMISI_INFO_SHORTSTRING];		
} COMPUTERMODELINFO;

typedef struct _BASEBOARDINFO
{
	wchar_t	szBaseBoard_Manufacturer[MAXBASEBOARDS][WMISI_INFO_SHORTSTRING];	
	wchar_t	szBaseBoard_Product[MAXBASEBOARDS][WMISI_INFO_SHORTSTRING];			
	wchar_t	szBaseBoard_SerialNumber[MAXBASEBOARDS][WMISI_INFO_SHORTSTRING];	
	wchar_t	szBaseBoard_Version[MAXBASEBOARDS][WMISI_INFO_SHORTSTRING];			
} BASEBOARDINFO;

typedef struct _SERIALPORTINFO
{
	wchar_t	szSerialPort_DeviceID[MAXSERIALPORTS][WMISI_VSHORTSTRING];		
	wchar_t	szSerialPort_Description[MAXSERIALPORTS][WMISI_VSHORTSTRING];	
	wchar_t	szSerialPort_ProviderType[MAXSERIALPORTS][WMISI_VSHORTSTRING];	
	DWORD	dwSerialPort_MaxBaudRate[MAXSERIALPORTS];							
} SERIALPORTINFO;

typedef struct _PARALLELPORTINFO
{
	wchar_t	szParallelPort_Caption[MAXSPARALLELPORTS][WMISI_VSHORTSTRING];		
} PARALLELPORTINFO;

typedef struct _NETWORKADAPTERINFO
{
	wchar_t	szNetworkAdapter_PNPDeviceID[MAXNETWORKADAPTERS][WMISI_INFO_SHORTSTRING];		
	wchar_t	szNetworkAdapter_Description[MAXNETWORKADAPTERS][WMISI_INFO_SHORTSTRING];		
	wchar_t	szNetworkAdapter_Summary[MAXNETWORKADAPTERS][LONG_STRING_LEN];		
	UINT64	ui64Speed[MAXNETWORKADAPTERS];													
	wchar_t	szNetworkAdapter_MACAddress[MAXNETWORKADAPTERS][SHORT_STRING_LEN];		
} NETWORKADAPTERINFO;

///PortType 
#define Win32_PortConnector_PortType_PS2		 2
#define Win32_PortConnector_PortType_Keyboard	13
#define Win32_PortConnector_PortType_Mouse		14
#define Win32_PortConnector_PortType_USB		16
#define Win32_PortConnector_PortType_Firewire	17
#define Win32_PortConnector_PortType_Audio		29
#define Win32_PortConnector_PortType_NUM		33

typedef struct _PORTCONNECTORINFO
{
	DWORD	dwPortConnector_PortType[MAXPORTCONNECTORS];									
	wchar_t	szPortConnector_PortType[MAXPORTCONNECTORS][WMISI_SHORTSTRING];			
	wchar_t	szPortConnector_ConnectorType[MAXPORTCONNECTORS][WMISI_SHORTSTRING];		
	DWORD	dwPortConnector_NumUSBPorts;													
	DWORD	dwPortConnector_NumFirewirePorts;												
} PORTCONNECTORINFO;

typedef struct _DISKPARTITIONINFO
{
	/// Number of partitions for the physical drive indicated by index, e.g. [0] = 4 means Physicaldisk0 has 4 partitions
	int		iNumDiskPartitions[MAXNUMDISKS];			
} DISKPARTITIONINFO;

typedef struct _SYSINFO_WMI 
{
	BIOSINFO			BIOSInfo;	
	VIRTUALMEMORYINFO	VirtualMemoryInfo;
	OPTICALDRIVEINFO	OpticalDriveInfo;
	PHYSICALDRIVEINFO	PhyscialDriveInfo;
	DISKVOLUMEINFO		DiskVolumeInfo;
	RAMINFO_WMI			RAMInfo;
	COMPUTERMODELINFO	ComputerModelInfo;
	BASEBOARDINFO		BaseBoardInfo;
	SERIALPORTINFO		SerialPortInfo;
	PARALLELPORTINFO	ParallelPortInfo;
	NETWORKADAPTERINFO	NetworkAdapterInfo;
	PORTCONNECTORINFO	PortConnectorInfo;
	DISKPARTITIONINFO	DiskPartitionInfo;

} SYSINFO_WMI;

/*! 
  \brief		Call this function to return system information via WMI 
				(Windows Management Information).  
		 
				The call is synchronous and can take some time to return.

  \note			This will initialize the COM object library for the current thread. 
				As such, if you have already initialized COM, this will fail. 
				Either run in a thread, use the #SysInfoDll_GetSystemInfo_WMI_async version, 
				or close the currently open COM library.
  \param[out]	SysInfo		Pointer to a structure to store the system information
  \return		TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL	SysInfoDll_GetSystemInfo_WMI ( SYSINFO_WMI	*SysInfo);

/*! 
  \brief		Call this function to return system information via WMI 
				(Windows Management Information).  
		 
				The call is asynchronous as it can take some time to return. 
		 
  \remarks		After calling this function, you will need to call 
				#SysInfoDll_GetSystemInfo_WMI_async_complete to check that the 
				system information is available. If it is complete, then the 
				information will be returned with the call to 
				#SysInfoDll_GetSystemInfo_WMI_async_complete.
  \param[out]	SysInfo_WMI		Pointer to a structure to store the system information
  \return		TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL	SysInfoDll_GetSystemInfo_WMI_async (SYSINFO_WMI *SysInfo_WMI);


/*! 
  \brief		Call this function to check whether the system information
				from a previous call to #SysInfoDll_GetSystemInfo_WMI_async
				is available.
		 
				If it is complete, then the information will be returned with this call.
  \param[out]	SysInfo				Pointer to a structure to store the system information
  \param[out]	iProgressPercent	Pointer to an integer to store the current percent progress
  \return		TRUE if the system information is successfully retrieved, otherwise FALSE
*/
SYSINFO_API BOOL	SysInfoDll_GetSystemInfo_WMI_async_complete (SYSINFO_WMI *SysInfo , int *iProgressPercent);


/*! 
  \brief		Call this function to return system physcial disk information via WMI (Windows Management Information).  
				This is a subset of the SysInfoDll_GetSystemInfo_WMI API. The call is synchronous.

  \note			This will initialize the COM object library for the current thread. 
				As such, if you have already initialized COM, this will fail. 
				Either run in a thread, or close the currently open COM library.
  \param[out]	PhyscialDriveInfo		Pointer to a structure to store the physcial disk information
  \return		TRUE if successful, otherwise FALSE
*/
int SysInfoDll_GetHDDetails(PHYSICALDRIVEINFO *PhyscialDriveInfo);


/*! 
  \brief		Call this function to get the physical disk system information. 
				This is a subset of the SysInfoDll_GetSystemInfo_WMI() API.
  \param[in]	SysInfo_WMI: Pointer to the WMI system information.
  \param[out]	line: The summary line string.
  \param[out]	iLen: The length of the summary line string.
  \return		TRUE if the system information is successfully retrieved, otherwise FALSE
*/
SYSINFO_API wchar_t * SysInfoDll_GetHDDSummaryLine(SYSINFO_WMI *SysInfo_WMI, wchar_t * line, int iLen);


/** @} */ // end of wmi_group




/** @defgroup imapi_group Optical disk Burner (IMAPI) APIs
 *  @{
 */

#define MAXBURNERSTRLEN		60
#define IMAPI2_MAX_FEATURES	70

typedef struct _IMAPI_INFO_DRIVE
{
	TCHAR	szVolume[MAXBURNERSTRLEN];	//drive volume 
	TCHAR	szVendor[MAXBURNERSTRLEN];	//drive vendor
	TCHAR	szProduct[MAXBURNERSTRLEN];	//drive product 
	int		iFeatures[IMAPI2_MAX_FEATURES];		//features for drive
	TCHAR	szFeatureSummary[LONG_STRING_LEN];	//drive product 
} IMAPI_INFO_DRIVE;

typedef struct _IMAPI_INFO
{
	int					iNumDrives;
	IMAPI_INFO_DRIVE	Drive[MAXNUMCDDVD];
} IMAPI_INFO;

/*! 
  \brief			Call this function to retrieve a information about all of the IMAPI 2 optical drive burners on the system
  \param[out]		szIMAPI2BurnersSummary Summary string for all Optical drives on the system.
  \param[out]		szIMAPI2Burners Summary strings for the Optical drives.
  \param[out]		pIMAPI_Drives pointer to IMAPI System Information retrieved.
*/
HRESULT SysInfoDll_GetIMAPI2OpticalBurners(wchar_t szIMAPI2BurnersSummary[MAXBURNERSTRLEN], wchar_t szIMAPI2Burners[MAXNUMCDDVD][MAXBURNERSTRLEN], IMAPI_INFO *pIMAPI_Drives);
/** @} */ // end of imapi_group




/** @defgroup smbios_group SMBIOS APIs 
 *  @{
 */
// defines
#define SMB_VSHORT_STRINGLEN		32
#define SMB_STRINGLEN				64
// SMBIOS structs
typedef struct _RAMINFO_SMBIOS
{
	int		iNumberRAMModules;
	ULONGLONG ullRAM_Capacity[MAXRAMMODULES];
	DWORD	dwRAM_MemoryType[MAXRAMMODULES];	
	DWORD	dwRAM_PositionInRow[MAXRAMMODULES];	
	wchar_t	szRAM_MemoryLocation[MAXRAMMODULES][SMB_VSHORT_STRINGLEN];	
	DWORD	dwRAM_Speed[MAXRAMMODULES];			
	wchar_t	szRAM_Manufacturer[MAXRAMMODULES][SMB_VSHORT_STRINGLEN];	
	wchar_t	szRAM_szSerialNumber[MAXRAMMODULES][SMB_STRINGLEN];	
	wchar_t	szRAM_RAMModuleHeading[MAXRAMMODULES][SMB_VSHORT_STRINGLEN];	
	wchar_t	szRAM_RAMModuleLine1[MAXRAMMODULES][WMISI_INFO_VSHORTSTRING];	
	wchar_t	szRAM_RAMModuleLine2[MAXRAMMODULES][WMISI_INFO_VSHORTSTRING];	
} RAMINFO_SMBIOS;

typedef struct _SYSINFO_SMBIOS
{
	BIOSINFO			BIOSInfo;	
	RAMINFO_SMBIOS		RAMInfo;
} SYSINFO_SMBIOS;

enum SMB_STATUS {
	SMBERR_SUCCESS,
	SMBERR_MEMALLOC,
	SMBERR_INIT_DIRECTIO,
	SMBERR_NO_SMBIOS,
	SMBERR_ACCESS_DENIED,
	SMBERR_FAILURE,
};


/*! 
  \brief		Call this function to return system information via SMBIOS (System Management BIOS).
  \param[out]	SysInfo		Pointer to a structure to store the SMBIOS information
  \return		0 if successful, otherwise an error code (see #SMB_STATUS)
*/
SYSINFO_API int				SysInfoDll_GetSystemInfo_SMBIOS( SYSINFO_SMBIOS	*SysInfo );

/** @} */ // end of smi_group




/** @defgroup partition_info_group Disk Partition Information APIs
 *  @{
 */

// defines
///Partition types
///Unknown.  
#define PART_UNKNOWN	0x00		
///12-bit FAT.  
#define PART_DOS2_FAT	0x01		
///16-bit FAT. Partition smaller than 32MB.  
#define PART_DOS3_FAT	0x04		
///Extended MS-DOS Partition.  
#define PART_EXTENDED	0x05		
///16-bit FAT. Partition larger than or equal to 32MB.  
#define PART_DOS4_FAT	0x06		
///NTFS
#define PART_NTFS		0x07		
///32-bit FAT. Partition up to 2047GB.  
#define PART_DOS32		0x0B			
///Same as PART_DOS32(0Bh), but uses Logical Block Address Int 13h extensions.  
#define PART_DOS32X		0x0C		
///Same as PART_DOS4_FAT(06h), but uses Logical Block Address Int 13h extensions.  
#define PART_DOSX13		0x0E		
///Same as PART_EXTENDED(05h), but uses Logical Block Address Int 13h extensions.  
#define PART_DOSX13X	0x0F	
///Dynamic disk
#define PART_DYNAMICDISK 0x42
///Linux partition (eg. ext2, ext3)
#define PART_LINUX		0x83
///Linux partition LVM (eg. ext2, ext3)
#define PART_LINUX_LVM	0x8E
///HFS or HFS+
#define PART_HFS		0xAF
///GPT disk (Protection MBR flag)
#define PART_GPT		0xEE
///Raw partition
#define PART_RAW		0xF0	

///Maximum number of partitions per physical disk supported for collecting disk partition information
#define MAX_PARTITIONS 256		//Arbitrary number
///Maximum number of physcial disk supported for collecting disk partition information
#define MAX_NUM_PHYSDISKS	(100 + 26 + 1) //100 physcial disks, 26 volumes with no physcial disk + 1 raw dd image
#define START_VOLUME_INDEX	100
#define START_IMAGE_INDEX	(100 + 26)

#define DRIVE_NOT_MOUNTED (DWORD) -1


// structs
typedef struct
{
	/// Number of the physical disk
	WORD		wPhyDisk;
	/// Start Cylinder/Head/Sector (CHS) for this partition
	WORD		wCylinder;
	WORD		wHead;
	WORD		wSector;
	/// Number of sectors in this partition
	ULONGLONG	ullNumSectors;		
	WORD		wType;
	/// Starting sector of this partition on the raw physical disk
	ULONGLONG	ullRelativeSector;
	ULONGLONG	ullNTRelativeSector;
	/// Bytes per sector for this partition
	DWORD		dwBytesPerSector;
	/// Bytes per cluster for this partition
	DWORD		dwBytesPerCluster;
	/// Bytes per Master File Table record for this partition
	WORD		wMFTRecordSz;
	///Drive volume letter (e.g. "C:")				
	///In the simple (most common case) of 1 to 1 Volume to partion mapping, store the drive letter for this partition (more complicated mappings not supported e.g. 1 -> 2 extents)
	TCHAR		szDrive[3];		
	///Volume type, e.g. PART_NTFS
	WORD		chType;
} DRIVEPACKET;

///Disk Type
///Unknown
#define UNKNOWN_DISK_TYPE 0
///Master Boot Record partition table
#define BASIC_DISK	      1
///EFI/ GUID Partition Table
#define DYNAMIC_DISK	  2


///Partition table types
///Unknown
#define UNKNOWN_DISK_PARTITION 0
///Master Boot Record partition table
#define MBR_DISK			  1
///EFI/ GUID Partition Table
#define GPT_DISK			  2
///Apple Partition Map
#define APM_DISK			  3

typedef struct
{
	///Disk type (Basic or Dynamic)
	WORD		nDiskType;
	///Partition table type
	WORD		nPartitionTableType;
	///Number of partitions described in the partition table for this physical drive (0 if no partition table)
	WORD		nNumPartitions;
	///image on a mounted phyical and logical drive e.g. image mount pro, or a disk with no partition table FAT16/FAT32 or NTFS
	bool		bNFTSImage;		
	///NTFS image on a mounted logical drive (no physcial drive number) 
	bool		bVolumeImage;	
	///NTFS image bit for bit raw image file, not mounted e.g. .dd image //Reserved - not used
	bool		bDDImage;		
	///Array of partition information for this physcial drive
	DRIVEPACKET PartitionInfo[MAX_PARTITIONS];
}PHYSDISK;


/*! 
  \brief        Call this function to return Disk Partition information from the Partition table and Master Boot Record for NTFS and FAT partitions.
  \param[in,out] PhyDisks: Pointer to an array of Physical disk descriptions. Note: The pointers must be NULL. If the pointers are not NULL, then this call is a refressh, and only disks that are new (e.g. newly enumerated USB drives) will be added.
  \return		ERROR_SUCCESS on success otherwise returns restult of GetLastError()
*/
SYSINFO_API DWORD SysInfoDll_GetPartitionInfo(PHYSDISK	*PhyDisks[MAX_NUM_PHYSDISKS]);


/*! 
  \brief        Call this function to return Disk Partition information via Partition table and Master Boot Record. 
  \param[in,out] PhyDisks: Pointer to an array of Physical disk descriptions (caller must delete memory)
                Note: The pointers must be NULL. If the pointers are not NULL, then this call is a refresh, and only disks that are new (e.g. newly enumerated USB drives) will be added. The caller must free the memory allocated .
  \return		ERROR_SUCCESS on success otherwise returns restult of GetLastError()
*/
SYSINFO_API DWORD SysInfoDll_GetPartitionInfoAll(PHYSDISK *PhyDisks[MAX_NUM_PHYSDISKS]);


/*! 
  \brief		Call this function to return Disk Partition information (for a single disk) via  
				Partition table and Master Boot Record.  Requries the disk to be opened before
				calling.
		 
  \param[in]     hDrive: Handle to the drive.
  \param[in]     wDrive: Physcial drive number.
  \param[in,out] Disk:   Pointer to an array of Physical disk descriptions (caller must delete memory)
				Note: The pointers must be NULL. If the pointers are not NULL, then this call is a refressh,
				and only disks that are new (e.g. newly enumerated USB drives) will be added. The caller
				must free the memory allocated .
  \return		ERROR_SUCCESS on success otherwise returns restult of GetLastError()
*/
/////////////////////////////////////////////////////////////////////////////////////////
SYSINFO_API DWORD SysInfoDll_ScanPartitionTableBlock(HANDLE hDrive, DWORD wDrive, PHYSDISK *Disk);


/** @} */ // end of partition_info_group





/** @defgroup all_group Aggregate APIs 
 *  @{
 */

/*! 
  \brief	  Call this function to get the aggregate of all of the system information calls above.

			  This is a synchronous function call.
  \param[out] szComputerName	Pointer to a string to store the computer name
  \param[out] szszOSName		Pointer to a string to store the OS information
  \param[out] pCPUinfo			Pointer to a structure to store the CPU information
  \param[out] pMemInfoEx		Pointer to a structure to store the memory information
  \param[out] MM				Pointer to a structure to store the monitor information
  \param[out] DD				Pointer to a structure to store the display device information
  \param[out] szGraphicsSummary Pointer to a string to store the monitor/display summary information
  \param[out] SysInfo_WMI		Pointer to a structure to store the WMI system information
  \param[out] SysInfo_SMBIOS	Pointer to a structure to store the SMBIOS system information
  \param[out] USBHostControllers Pointer to a structure to store the USB controller information
  \return     TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_GetAll( LPWSTR				szComputerName , 
											   LPWSTR				szszOSName ,
											   CPUINFO				*pCPUinfo ,
											   MEMORYSTATUSEX		*pMemInfoEx ,
											   MULTIMONITOR_DYN		*MM , 
											   DISPLAY_DEVICES		*DD , 
											   LPWSTR				szGraphicsSummary ,
											   SYSINFO_WMI			*SysInfo_WMI ,
											   SYSINFO_SMBIOS		*SysInfo_SMBIOS ,
											   USB_HOST_CONTROLLERS *USBHostControllers );


/*! 
  \brief      Call this function to get the aggregate of all of the system information calls above.

			  The call is asynchronous as it can take some time to return.

  \remarks	  After calling this function, all data will be returned except 
			  for the SysInfo_WMI. Use #SysInfoDll_GetSystemInfo_WMI_async_complete 
			  to check that the WMI system information is available. 
			  If it is complete, then the WMI information will be returned 
			  with the call to #SysInfoDll_GetSystemInfo_WMI_async_complete.
  \sa		  SysInfoDll_GetSystemInfo_WMI_async_complete
  \param[out] szComputerName	Pointer to a string to store the computer name
  \param[out] szszOSName		Pointer to a string to store the OS information
  \param[out] pCPUinfo			Pointer to a structure to store the CPU information
  \param[out] pMemInfoEx		Pointer to a structure to store the memory information
  \param[out] MM				Pointer to a structure to store the monitor information
  \param[out] DD				Pointer to a structure to store the display device information
  \param[out] szGraphicsSummary Pointer to a string to store the monitor/display summary information
  \param[out] SysInfo_WMI		Pointer to a structure to store the WMI system information
  \param[out] SysInfo_SMBIOS	Pointer to a structure to store the SMBIOS system information
  \param[out] USBHostControllers Pointer to a structure to store the USB controller information
  \return	  TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_GetAll_async( LPWSTR						szComputerName , 
													 LPWSTR						szszOSName ,
													 CPUINFO					*pCPUinfo ,
													 MEMORYSTATUSEX				*pMemInfoEx ,
													 MULTIMONITOR_DYN			*MM ,
													 DISPLAY_DEVICES			*DD , 
													 LPWSTR						szGraphicsSummary ,
													 SYSINFO_WMI				*SysInfo_WMI , 
													 SYSINFO_SMBIOS				*SysInfo_SMBIOS ,
													 USB_HOST_CONTROLLERS		*USBHostControllers );

/** @} */ // end of all_group





/** @defgroup report_group Report APIs 
 *  @{
 */
// structs
enum SYSINFO_FORMAT_TYPE {
	///Text:  Used for text file operations (like saving to text file)
	FORMAT_TEXT = 0,
	///HTML
	FORMAT_HTML,
	///Text:  Used for non text file operations (like clipboard copy)
	FORMAT_TEXT_CLIPBOARD,	
};

/*! 
  \brief	 Call this function to format all system information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] szComputerName		a string containing the computer name to include in the string
  \param[in] szOSName			a string containing the OS information to include in the string
  \param[in] pCPUinfo			Pointer to a structure containing the CPU info to include in the string
  \param[in] pMemInfoEx			Pointer to a structure containing the Memory info to include in the string
  \param[in] pMemModuleInfo		Pointer to a structure containing the Memory SPD info to include in the string
  \param[in] MM					Pointer to a structure containing the Monitor info to include in the string
  \param[in] DD					Pointer to a structure containing the Display device info to include in the string
  \param[in] szGraphicsSummary	a string containing the Monitor/display device summary to include in the string
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[in] SysInfo_SMBIOS		Pointer to a structure containing the SMBIOS info to include in the string
  \param[in] USBHostControllers Pointer to a structure containing the USB controller info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatAll( SYSINFO_FORMAT_TYPE	Format ,
												  LPWSTR				szComputerName , 
												  LPWSTR				szOSName ,
												  CPUINFO				*pCPUinfo , 
												  MEMORYSTATUSEX		*pMemInfoEx ,
												  MEMMODULEINFO			*pMemModuleInfo,
												  MULTIMONITOR_DYN		*MM , 
												  DISPLAY_DEVICES		*DD , 
												  LPWSTR				szGraphicsSummary ,
												  SYSINFO_WMI			*SysInfo_WMI ,
												  SYSINFO_SMBIOS		*SysInfo_SMBIOS ,
												  USB_HOST_CONTROLLERS	*USBHostControllers ,
												  LPWSTR				pszRes ,
												  int					*iResLen );

/*! 
  \brief	 Call this function to format all system information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] szComputerName		a string containing the computer name to include in the string
  \param[in] szOSName			a string containing the OS information to include in the string
  \param[in] pCPUinfo			Pointer to a structure containing the CPU info to include in the string
  \param[in] pMemInfoEx			Pointer to a structure containing the Memory info to include in the string
  \param[in] pMemModuleInfo		Pointer to a structure containing the Memory SPD info to include in the string
  \param[in] MM					Pointer to a structure containing the Monitor info to include in the string
  \param[in] DD					Pointer to a structure containing the Display device info to include in the string
  \param[in] szGraphicsSummary	a string containing the Monitor/display device summary to include in the string
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[in] SysInfo_SMBIOS		Pointer to a structure containing the SMBIOS info to include in the string
  \param[in] USBHostControllers Pointer to a structure containing the USB controller info to include in the string
  \param[in] pIMAPI_Drives		Pointer to a structure containing the IMAPI info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatAll2( SYSINFO_FORMAT_TYPE	Format ,
												  LPWSTR				szComputerName , 
												  LPWSTR				szOSName ,
												  CPUINFO				*pCPUinfo , 
												  MEMORYSTATUSEX		*pMemInfoEx ,
												  MEMMODULEINFO			*pMemModuleInfo,
												  MULTIMONITOR_DYN		*MM , 
												  DISPLAY_DEVICES		*DD , 
												  LPWSTR				szGraphicsSummary ,
												  SYSINFO_WMI			*SysInfo_WMI ,
												  SYSINFO_SMBIOS		*SysInfo_SMBIOS ,
												  USB_HOST_CONTROLLERS	*USBHostControllers ,
												  IMAPI_INFO			*pIMAPI_Drives,
												  LPWSTR				pszRes ,
												  int					*iResLen );

/*! 
  \brief	 Call this function to format all system information (summary only) into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] szComputerName		a string containing the computer name to include in the string
  \param[in] szOSName			a string containing the OS information to include in the string
  \param[in] pCPUinfo			Pointer to a structure containing the CPU info to include in the string
  \param[in] pMemInfoEx			Pointer to a structure containing the Memory info to include in the string
  \param[in] szGraphicsSummary	a string containing the Monitor/display device summary to include in the string
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[in] SysInfo_SMBIOS		Pointer to a structure containing the SMBIOS info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatSummary( SYSINFO_FORMAT_TYPE	Format ,
													  LPWSTR				szComputerName , 
													  LPWSTR				szOSName ,
													  CPUINFO				*pCPUinfo ,
													  MEMORYSTATUSEX		*pMemInfoEx ,
													  LPWSTR				szGraphicsSummary ,
													  SYSINFO_WMI			*SysInfo_WMI ,
													  SYSINFO_SMBIOS		*SysInfo_SMBIOS ,
													  LPWSTR				pszRes , 
													  int					*iResLen );

/*! 
  \brief	 Call this function to format all system information (summary only) into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] szComputerName		a string containing the computer name to include in the string
  \param[in] szOSName			a string containing the OS information to include in the string
  \param[in] pCPUinfo			Pointer to a structure containing the CPU info to include in the string
  \param[in] pMemInfoEx			Pointer to a structure containing the Memory info to include in the string
  \param[in] szGraphicsSummary	a string containing the Monitor/display device summary to include in the string
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[in] SysInfo_SMBIOS		Pointer to a structure containing the SMBIOS info to include in the string
  \param[in] pIMAPI_Drives		Pointer to a structure containing the IMAPI info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatSummary2(SYSINFO_FORMAT_TYPE Format,
													   wchar_t				*szComputerName, 
													   wchar_t				*szOSName,
													   CPUINFO				*pCPUinfo,
													   MEMORYSTATUSEX		*pMemInfoEx,
													   wchar_t				*szGraphicsSummary,
													   SYSINFO_WMI			*SysInfo_WMI,
													   SYSINFO_SMBIOS		*SysInfo_SMBIOS,
													   IMAPI_INFO			*pIMAPI_Drives,
													   wchar_t				*pszRes, 
													   int					*iResLen);

/*! 
  \brief	 Call this function to format the general system information into a string (for reporting purposes).
  \param[in] Format				formatting method (see SYSINFO_FORMAT_TYPE)
  \param[in] szComputerName		a string containing the computer name to include in the string
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[in] SysInfo_SMBIOS		Pointer to a structure containing the SMBIOS info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatGeneral( SYSINFO_FORMAT_TYPE	Format ,
													  LPWSTR				szComputerName , 
													  SYSINFO_WMI			*SysInfo_WMI ,
													  SYSINFO_SMBIOS		*SysInfo_SMBIOS ,
													  LPWSTR				pszRes , 
													  int					*iResLen );

/*! 
  \brief	 Call this function to format the PCI information into a string (for reporting purposes).
  \param[in] Format				formatting method (see SYSINFO_FORMAT_TYPE)
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatPCI( SYSINFO_FORMAT_TYPE	Format ,
												  LPWSTR				pszRes , 
												  int					*iResLen );

/*! 
  \brief	 Call this function to format the CPU information into a string (for reporting purposes).
  \param[in] Format				formatting method (see SYSINFO_FORMAT_TYPE)
  \param[in] pCPUinfo			Pointer to a structure containing the CPU info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatCPU( SYSINFO_FORMAT_TYPE	Format ,
												  CPUINFO				*pCPUinfo ,
												  LPWSTR				pszRes , 
												  int					*iResLen );

/*! 
  \brief	 Call this function to format the Memory information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] pMemInfoEx			Pointer to a structure containing the Memory info to include in the string
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[in] SysInfo_SMBIOS		Pointer to a structure containing the SMBIOS info to include in the string
  \param[in] pMemModuleInfo		Pointer to a structure containing the memory SPD information
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatMemory( SYSINFO_FORMAT_TYPE	Format ,
													 MEMORYSTATUSEX			*pMemInfoEx ,
													 SYSINFO_WMI			*SysInfo_WMI ,
													 SYSINFO_SMBIOS			*SysInfo_SMBIOS ,
													 MEMMODULEINFO			*pMemModuleInfo ,
													 LPWSTR					pszRes , 
													 int					*iResLen );

/*! 
  \brief	 Call this function to format the Memory SPD information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] pMemModuleInfo		Pointer to a structure containing the Memory SPD info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatMemSPD( SYSINFO_FORMAT_TYPE	Format ,
													 MEMMODULEINFO			*pMemModuleInfo ,
													 LPWSTR					pszRes , 
													 int					*iResLen );
/*! 
  \brief	 Call this function to format the Monitor/Display Device information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] MM					Pointer to a structure containing the Monitor info to include in the string
  \param[in] DD					Pointer to a structure containing the Display device info to include in the string
  \param[in] szGraphicsSummary	a string containing the Monitor/display device summary to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatGraphics( SYSINFO_FORMAT_TYPE	Format,
													   MULTIMONITOR_DYN		*MM,
													   DISPLAY_DEVICES		*DD, 
													   LPWSTR				szGraphicsSummary,
													   LPWSTR				pszRes, 
													   int					*iResLen);

/*! 
  \brief	 Call this function to format the Disk volume information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatDiskVolumes ( SYSINFO_FORMAT_TYPE	Format ,
														   SYSINFO_WMI			*SysInfo_WMI ,
														   LPWSTR				pszRes , 
														   int					*iResLen );

/*! 
  \brief	 Call this function to format the Disk drive information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatDiskDrives( SYSINFO_FORMAT_TYPE	Format ,
														 SYSINFO_WMI			*SysInfo_WMI ,
														 LPWSTR					pszRes , 
														 int					*iResLen );

/*! 
  \brief	 Call this function to format the Optical drive information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatOpticalDrives( SYSINFO_FORMAT_TYPE	Format,
														    SYSINFO_WMI			*SysInfo_WMI,
														    LPWSTR				pszRes, 
														    int					*iResLen);

/*! 
  \brief	 Call this function to format the Optical drive information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[in] pIMAPI_Drives		Pointer to a structure containing the IMAPI info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatOpticalDrives2(SYSINFO_FORMAT_TYPE Format,
															 SYSINFO_WMI *SysInfo_WMI,
															 IMAPI_INFO *pIMAPI_Drives,
															 wchar_t *pszRes, 
															 int *iResLen);

/*! 
  \brief	 Call this function to format the Network information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatNetwork( SYSINFO_FORMAT_TYPE	Format,
													  SYSINFO_WMI			*SysInfo_WMI,
													  LPWSTR				pszRes, 
													  int					*iResLen);

/*! 
  \brief	 Call this function to format the Port information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] SysInfo_WMI		Pointer to a structure containing the WMI info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatPorts( SYSINFO_FORMAT_TYPE	Format,
												    SYSINFO_WMI			*SysInfo_WMI,
													LPWSTR				pszRes,
													int					*iResLen);

/*! 
  \brief	 Call this function to format the USB controller and device information into a string (for reporting purposes).
  \param[in] Format				formatting method (see #SYSINFO_FORMAT_TYPE)
  \param[in] USBHostControllers Pointer to a structure containing the USB controller info to include in the string
  \param[out] pszRes			a string buffer to store the formatted string
  \param[in,out] iResLen		Pointer to the size of the pszRes string buffer. On return, contains the length of the pszRes string
  \return	 TRUE if successful, otherwise FALSE
*/
SYSINFO_API BOOL			SysInfoDll_FormatUSB( SYSINFO_FORMAT_TYPE	Format,
												  USB_HOST_CONTROLLERS	*USBHostControllers,
												  LPWSTR				pszRes, 
												  int					*iResLen);

/** @} */ // end of report_group





/*=======================================================================
SysInfo DLL SMART APIs

   The SMART APIs is a self-contained subcomponent of the SysInfo DLL
   for retrieving SMART information from supported hard disks. This set of
   APIs was formerly released as a standalone DLL under the name
   "PassMark SmartDisk DLL SDK".
   
   These APIs are largely independant of the other SysInfo APIs and 
   therefore are prefixed with "SysInfo_Smart" as a distinguishing
   marker. Any initialization, cleanup, debugging, reporting functions are
   solely for use with the SMART APIs and are not associated at all with
   the other SysInfo APIs.
=======================================================================*/

/** @defgroup smart_group SMART APIs
 *  @{
 */

// defines
/*--- General --- */
#define SERIAL_NUM_LEN						20	
#define FIRMWARE_REV_LEN					8
#define MODEL_NUM_LEN						40
#define	VENDOR_UNIQUE_LEN					3
#define WWN_LEN								8

#define	MAX_PARTITIONS_PER_DEVICE			8

/** @defgroup smart_err_group SMART Return Codes
 *  @{
 */
/// No error.  Function completed and returned successfully.
#define	SMART_STATUS_SUCCESS						0

/// The device ID passed in does not exist.  Please use GetSmartDeviceIDFirst and GetSmartDeviceIDNext to get the device ID.
#define	SMART_ERR_DEVICE_ID_DNE						-1000
/// Disk does not support S.M.A.R.T.  Older disk that does not support S.M.A.R.T. will get this error.
#define	SMART_ERR_DEVICE_ID_NOT_SMART				-1001
#define	SMART_ERR_DEVICE_OPEN						-1002
/// Valid drive numbers are from 0 ("A:\") to 25 ("Z:\").  Any number not in this range will yield this error.
#define	SMART_ERR_DRIVE_NUMBER_INVALID				-1100
/// Drive number does not exist.  This means there is not physical device that has this drive number.
#define	SMART_ERR_DRIVE_NUMBER_DNE					-1101
/// Drive number is not a fixed drive.  Floppies and CDROMs are not fixed drives and will get this error.
#define	SMART_ERR_DRIVE_NUMBER_NOT_FIXED			-1102

/// Buffer provided points to NULL.
#define	SMART_ERR_BUFFER_INVALID					-2000
/// Buffer provided is too small to hold all the data.
#define	SMART_ERR_BUFFER_TOO_SMALL					-2001
/// Failed to allocate buffer in function.
#define	SMART_ERR_BUFFER_FAILED_ALLOCATE			-2002

/// There is no S.M.A.R.T. device ID in the list.
#define	SMART_ERR_SMART_DEVICE_ID_EMPTY				-3001
/// We have reached the end of list for the S.M.A.R.T. device ID(s).
#define	SMART_ERR_SMART_DEVICE_ID_EOL				-3002

/// There is no S.M.A.R.T. attribute for the drive.
#define	SMART_ERR_SMART_ATTR_ID_EMPTY				-4001
/// We have reached the end of list for the S.M.A.R.T. attribute(s) for the drive.
#define	SMART_ERR_SMART_ATTR_ID_EOL					-4002
/// Smart attribute ID is invalid.  Attribute ID cannot be zero.
#define	SMART_ERR_SMART_ATTR_ID_INVALID				-4003
/// Smart attribute ID unrecognized by DLL.  Possibly a new attribute implemented by certain hard disk manufacturer.  The attribute value will still be valid.  However, you will get "Unknown Attribute" if you try and get its description string.  This is a warning rather than an error.
#define	SMART_WARN_SMART_ATTR_ID_UNKNOWN			-4004

/// Fail to open file for reading and writing.  This happens when the Windows API fopen() returned an error.
#define	SMART_ERR_FOPEN_FAIL						-5001
/// Fail to write to file.  This happens when the number of characters written using the Windows API fwrite() is less than what we intended to write.
#define	SMART_ERR_FWRITE_FAIL						-5002
/// Fail to read file.  This happens when the Windows API fread() returned an error.
#define	SMART_ERR_FREAD_FAIL						-5003
/// No read/write permission for file.
#define	SMART_ERR_ACCESS_RW							-5101
/// No read permission for file.
#define	SMART_ERR_ACCESS_RO							-5102
/// Fail to seek to a location in file.  This happens when the Windows API fseek() returned an error.
#define	SMART_ERR_FSEEK_FAIL						-5201

/// Fail to get device capacity.
#define	SMART_ERR_GET_PHY_CAPACITY					-6001

/// No access rights when trying to get info for drive.
#define	SMART_ERR_BUILD_DRIVES_INFO_NO_ACCESS		-7001
/// No access rights to gather drive information.  In Windows Server and Vista, your application needs to "Run as administrator".
#define	SMART_ERR_NO_ADMIN_RIGHTS					-7002

/// Failed to execute the specifield self test. The self test may not be supported.
#define SMART_ERR_SELFTEST_FAILED					-8001

/// Occur when you try and use SmartDisk DLL’s APIs without first calling SmartDisk Init.
#define	SMART_ERR_DLL_NOT_INIT						-10001
/// SmartDisk Init failed to allocate memory.
#define	SMART_ERR_DLL_INIT_MEMORY_ALLOC_FAIL		-10002
/// This function is not supported in Windows 98/Me.
#define	SMART_ERR_FUNCTION_NOT_SUPPORTED			-10003
/// Empty string returned from SmartDll GetXXX functions.
#define	SMART_ERR_EMPTY_STRING						-10004
/// One of the parameters is invalid.
#define	SMART_ERR_INVALID_PARAM						-10005

/// There are no S.M.A.R.T. attributes to record for the chosen drive number.
#define	SMART_ERR_TEC_NO_ATTRIBUTES					-20001
/// The TEC file does not have "SMDL" in the first 4 bytes of the file stream.
#define	SMART_ERR_TEC_SIGNATURE_HEAD				-20002
/// The TEC file does not have ""LDMS" in the last 4 bytes of the file stream.
#define	SMART_ERR_TEC_SIGNATURE_TAIL				-20003
/// The TEC file for the drive number does not exist.
#define	SMART_ERR_TEC_FILE_DNE						-20004
/// There are not enough TEC data to predict TEC.  We need to have at least 3 sets of data.
#define	SMART_WARN_TEC_NOT_ENOUGH_DATA				-20005
/// For given attribute ID, threshold value is 0.  It means we have no threshold value to work with to predict TEC.
#define	SMART_WARN_TEC_ATTR_ID_NA					-20006
/// No changes in attribute values have been detected, making it impossible to predict TEC.
#define	SMART_WARN_TEC_UNABLE_TO_PREDICT			-20007

#define	SMART_ERR_UNEXPECTED						-30001

/** @} */ // end of smart_err_group

/** @defgroup attr_id_group Attribute IDs
 *  @{
 */
#define	ATTR_INVALID						0x0
///Raw Read Error Rate: Represents the rate of uncorrected read errors.  Lower values indicate that there is a problem with either disk surface or read/write heads.
#define ATTR_READ_ERROR_RATE				0x1
///Throughput Performance: Represents the throughput performance of the drive, i.e. The speed at which the drive is reading and writing data.
#define ATTR_THROUGHPUT_PERF				0x2
///Spin Up Time: Represents the average amount of time required to spin up the drive spindle to operational speed from a stopped state.
#define ATTR_SPIN_UP_TIME					0x3
///Start/Stop Count: Represents the number of start/stop cycles for the drive.  The raw value indicates the count of start/stop cycles for the drive.  The drive being powered on/off or suspended/woken up are considered as start/stop cycles.
#define ATTR_START_STOP_COUNT				0x4
///Reallocated Sector Count: Represents the amount of spare sector pool available.  Spare sectors are used to replace sectors that became bad for some reason (for instance, if a read error occurs).  Therefore the more sectors reallocated, the worse the condition of the drive.  A high value represents few reallocated sectors, a low value represents a disk in poor physical condition.
#define ATTR_REALLOC_SECTOR_COUNT			0x5
///Read Channel Margin: Margin of a channel while reading data. The function of this attribute is not specified.
#define ATTR_READ_CHANNEL_MARGIN			0x6
///Seek Error Rate: Represents the number of seek errors.  Each time the drive attempts a seek operation, but fails to position its head correctly, the seek error rate increases.
#define ATTR_SEEK_ERROR_RATE				0x7
///Seek Time Performance: Represents how efficiently the drive is performing seek operations.  A low value indicates problems with the drive subsystem, for instance the servo responsible for positioning the head.
#define ATTR_SEEK_TIME_PERF					0x8
///Power On Hours Count: This is an informative attribute, the raw value of which displays the number of hours the drive has been powered on for.
#define ATTR_POWER_ON_TIME					0x9
///Spin Retry Count: Represents the number of times a drive fails to spin its spindle up to operation speed on the first attempt.  The lower the value, the more retries have occurred.
#define ATTR_SPIN_RETRY_COUNT				0xA
///Calibration Retry Count: Calibration is the act of repositioning the drive read/write head to cylinder 0.  This value represents the number of times a calibration has failed on the first attempt.
#define ATTR_CALIBRATION_RETRY_COUNT		0xB
///Power Cycle Count: Informative attribute, the raw value of which represents the number of drive power on/drive power off cycles for the disk.
#define ATTR_POWER_CYCLE_COUNT				0xC
///Soft Read Error Rate: Uncorrected read errors reported to the operating system.
#define ATTR_SOFT_READ_ERROR_RATE			0xD

#define ATTR_DOWNSHIFT_ERROR_COUNT			0xB7
#define ATTR_END_TO_END_ERROR				0xB8
#define ATTR_HEAD_STABILITY					0xB9
#define ATTR_INDUCED_OP_VIB_DETECT			0xBA
#define ATTR_UNCORRECTABLE_ERRORS			0xBB
#define ATTR_COMMAND_TIMEOUT				0xBC
#define ATTR_COMMAND_HIGH_FLY_WRITES		0xBD				
///Temperature Difference from 100: The exact specification of this attribute is not quite stable. This is what it is known as on Seagate drives whereas it is called Airflow Temperature on Western Digital hard disks. In either case this attribute usually has to do with subtracting the temperature from 100 to give a value that is worse when it is lower.
#define ATTR_100_MINUS_TEMPERATURE			0xBE
///G-Sense Error Rate: Frequency of mistakes as a result of impact loads
#define ATTR_GSENSE_ERROR_RATE_BF			0xBF

///Power-off Retract Count: Number of times the heads are loaded off the media. Heads can be unloaded without actually powering off.
#define ATTR_POWER_OFF_RETRACT_COUNT		0xC0
///Load/Unload Cycle: Count of load/unload cycles into head landing zone position
#define ATTR_LOAD_CYCLE_COUNT				0xC1
///Temperature: Informative attribute, the raw value of which represents the current temperature in Celsius of the drive.
#define ATTR_TEMPERATURE_C2					0xC2
///Hardware ECC Recovered: Time between ECC-corrected errors.
#define ATTR_HARDWARE_ECC_RECOVERED			0xC3
///Reallocation Event Count: Represents the number of reallocation events, which have taken place.  Sometimes multiple sectors are reallocated together – this corresponds to one reallocation event.  (See also: Reallocated Sector Count and Current Pending Sector Count).
#define ATTR_REALLOC_EVENT_COUNT			0xC4
///Current Pending Sector Count: Represents the number of sectors currently pending reallocation.
#define ATTR_PENDING_SECTOR_COUNT			0xC5
///Uncorrectable Sector Count: The total number of uncorrectable errors when reading/writing a sector.
#define ATTR_UNCORRECTABLE_SECTOR_COUNT		0xC6
///Ultra ATA CRC Error Count: Represents the number of CRC error found in the Ultra DMA high-speed transfer mode.  (CRC stands for Cyclic Redundancy Check and is data verification algorithm which uses polynomial checksums).
#define ATTR_ULTRADMA_CRC_ERROR_COUNT		0xC7
///Write Error Count: Represents the rate of uncorrected write errors.  Lower values indicate that there is a problem with either disk surface or read/write heads.
#define ATTR_WRITE_ERROR_COUNT				0xC8
///Soft Read Error Rate: Number of off-track errors.
#define ATTR_OFF_TRACK_ERRORS				0xC9
///Data Address Mark errors: Number of Data Address Mark errors.
#define ATTR_DATA_ADDRESS_MARK_ERRORS		0xCA
///Run Out Cancel: Number of ECC errors.
#define ATTR_ECC_ERRORS						0xCB
///Soft ECC Correction: Number of errors corrected by software ECC.
#define ATTR_SOFT_ECC_CORRECTION			0xCC
///Thermal asperity rate (TAR ): Number of thermal asperity errors.
#define ATTR_THERMAL_ASPERITY_RATE			0xCD
///Flying Height: Height of heads above the disk surface.
#define ATTR_FLYING_HEIGHT					0xCE
///Spin High Current: Amount of high current used to spin up the drive.
#define ATTR_SPIN_HIGH_CURRENT				0xCF

///Spin Buzz: Number of buzz routines to spin up the drive.
#define ATTR_SPIN_BUZZ						0xD0
///Offline Seek Performance: Drive’s seek performance during offline operations.
#define ATTR_OFFLINE_SEEK_PERFORMANCE		0xD1
#define ATTR_VIBRATION_DURING_WRITE			0xD3
#define ATTR_SHOCK_DURING_WRITE				0xD4
///Disk Shift: Distance the disk has shifted relative to the spindle (usually due to shock).
#define ATTR_DISK_SHIFT						0xDC
///GSense Error rate: The number of errors resulting from externally-induced shock & vibration.
#define ATTR_GSENSE_ERROR_RATE_DD			0xDD
///Loaded Hours: Time spent operating under data load (movement of magnetic head armature)
#define ATTR_LOADED_HOURS					0xDE
///Load/Unload retry count: Number of times head changes position.
#define ATTR_LOAD_UNLOAD_RERTY_COUNT		0xDF

///Load Friction: Resistance caused by friction in mechanical parts while operating.
#define ATTR_LOAD_FRICTION					0xE0
///Load/Unload Cycle Count: Total number of load cycles
#define ATTR_LOAD_UNLOAD_CYCLE_COUNT		0xE1
///Load 'In'-time: Total time of loading on the magnetic heads actuator (time not spent in parking area).
#define ATTR_LOAD_IN_TIME					0xE2
///Torque Amplification Count: Number of attempts to compensate for platter speed variations.
#define ATTR_TORQUE_AMPLIFICATION_COUNT		0xE3
///Power off retract cycle: The number of times the magnetic armature was retracted automatically as a result of cutting power.
#define ATTR_POWEROFF_RETRACT_CYCLE			0xE4
///GMR Head Amplitude: Amplitude of "thrashing" (distance of repetitive forward/reverse head motion).
#define ATTR_GMR_HEAD_AMPLITUDE				0xE6
///Temperature: Drive Temperature, unknown units.
#define ATTR_TEMPERATURE_E7					0xE7

///Head Flying Hours: Time while head is positioning.
#define ATTR_HEAD_FLYING_HOURS				0xF0
#define ATTR_TOTAL_LBAS_WRITTEN				0xF1
#define ATTR_TOTAL_LBAS_READ				0xF2
///Read Error Retry Rate: Number of errors while reading from a disk
#define ATTR_READ_ERROR_RETRY_RATE			0xFA
#define ATTR_FREE_FALL_PROTECTION			0xFE

/** @} */ // end of attr_id_group

/*--- For SysInfoDll_Smart_GetSmartAttributeByID --- */
///A text description of the attribute.  See below for a more detailed description of each attribute.
#define	ATTR_TYPE_STR_DESC					0x20
///Provided a status for the attribute.  OK means the attribute value is well in the safe zone.  FAIL means that a Threshold Exceeding Condition has been detected.
#define	ATTR_TYPE_INT_STATUS				0x40

///The current normalized attribute value.
#define	ATTR_TYPE_INT_VALUE					0x01
//The attributes current raw value.  This may be a count, a number of errors or hours, or even a temperature, depending on the attribute.
#define	ATTR_TYPE_ULONGLONG_RAW				0x02
///The worst (lowest) value recorded so far.  This is an indicator of how close to failure the drive has ever been.
#define	ATTR_TYPE_INT_WORST					0x04
///The attribute threshold.  This value will not change and represents the lowest possible safe attribute value.
#define	ATTR_TYPE_INT_THRESHOLD				0x08

enum SMART_SELFTEST
{
	///Abort off-line mode self-test routine
	SMART_SELFTEST_ABORT		= 0,
	///SMART Short self-test routine
	SMART_SELFTEST_SHORT		= 1,
	///SMART Extended self-test routine
	SMART_SELFTEST_EXT			= 2,
	///SMART Conveyance self-test routine
	SMART_SELFTEST_CONVEYANCE	= 3,
	///SMART Selective self-test routine
	SMART_SELFTEST_SELECTIVE	= 4,
	SMART_SELFTEST_MAX			= 5
};

///Indicates a previous self-test routine completed without error or no self-test status is available
#define SMART_SELFTEST_STATUS_NOERR					0x0
///The self-test routine was aborted by the host
#define SMART_SELFTEST_STATUS_ABORTED				0x1
///The self-test routine was interrupted by the host with a hardware or software reset
#define SMART_SELFTEST_STATUS_INTERRUPTED			0x2
///A fatal error or unknown test error occurred while the device was executing its self-test routine and the device was unable to complete the self-test routine.
#define SMART_SELFTEST_STATUS_FATALERR				0x3
///The previous self-test completed having a test element that failed and the test element that failed is not known.
#define SMART_SELFTEST_STATUS_FAILED_UNKNOWN		0x4
///The previous self-test completed having the electrical element of the test failed.
#define SMART_SELFTEST_STATUS_FAILED_ELECTRICAL		0x5
///The previous self-test completed having the servo and/or seek test element of the test failed.
#define SMART_SELFTEST_STATUS_FAILED_SERVOSEEK		0x6
///The previous self-test completed having the read element of the test failed.
#define SMART_SELFTEST_STATUS_FAILED_READ			0x7
///The previous self-test completed having a test element that failed and the device is suspected of having handling damage.
#define SMART_SELFTEST_STATUS_FAILED_HANDLING		0x8
///Self-test routine in progress.
#define SMART_SELFTEST_STATUS_IN_PROGRESS			0xF


/** @defgroup devinfo_group Device attributes
 *  @{
 */

// bInterfaceType
#define INTERACE_TYPE_UNKNOWN						0x00
#define INTERACE_TYPE_SCSI							0x01
#define INTERACE_TYPE_ATAPI							0x02
#define INTERACE_TYPE_ATA							0x03
#define INTERACE_TYPE_1394							0x04
#define INTERACE_TYPE_SSA							0x05
#define INTERACE_TYPE_FIBRE							0x06
#define INTERACE_TYPE_USB							0x07
#define INTERACE_TYPE_RAID							0x08
#define INTERACE_TYPE_ISCSI							0x09
#define INTERACE_TYPE_SAS							0x0A
#define INTERACE_TYPE_SATA							0x0B


/*--- DEVICEINFO.bATAVersionSupported --- */ 
#define ATA_COMPLIANCE(x)							(x != 0x0000 && x != 0xFFFF)
#define ATA8_SUPPORTED(x)							(x & (1 << 8))
#define ATA7_SUPPORTED(x)							(x & (1 << 7))
#define ATA6_SUPPORTED(x)							(x & (1 << 6))
#define ATA5_SUPPORTED(x)							(x & (1 << 5))
#define ATA4_SUPPORTED(x)							(x & (1 << 4))

/*--- DEVICEINFO.bTransportTypeSupported --- */
#define TRANSPORT_TYPE_AVAILABLE(x)					(x != 0x0000 && x != 0xFFFF)
#define TRANSPORT_TYPE_SERIAL(x)					(x & (1 << 12))
#define TRANSPORT_TYPE_PARALLEL(x)					((x & (1 << 12)) == 0)
#define SATA3_0_COMPLIANCE(x)						(x & (1 << 5))
#define SATA2_6_COMPLIANCE(x)						(x & (1 << 4))
#define SATA2_5_COMPLIANCE(x)						(x & (1 << 3))
#define SATA2_EXT_COMPLIANCE(x)						(x & (1 << 2))
#define SATA1_COMPLIANCE(x)							(x & (1 << 1))
#define ATA7_COMPLIANCE(x)							(x & (1 << 1))
#define ATA8_COMPLIANCE(x)							(x & 1)

/*--- DEVICEINFO.wMediaRotationRate ---*/
#define ROTATION_RATE_AVAILABLE(x)					(x != 0)
#define ROTATION_RATE_SSD(x)						(x == 0x0001)

/*--- DEVICEINFO.bMultiwordDMASupported ---*/
#define MULTIDMA_MODE2_SUPPORTED(x)					(x & (1 << 2))
#define MULTIDMA_MODE1_SUPPORTED(x)					(x & (1 << 1))
#define MULTIDMA_MODE0_SUPPORTED(x)					(x & 1)

/*--- DEVICEINFO.bMultiwordDMASelected ---*/
#define MULTIDMA_MODE2_SELECTED(x)					(x & (1 << 2))
#define MULTIDMA_MODE1_SELECTED(x)					(x & (1 << 1))
#define MULTIDMA_MODE0_SELECTED(x)					(x & 1)

/*--- DEVICEINFO.bUltraDMASupported ---*/
#define ULTRADMA_MODE6_SUPPORTED(x)					(x & (1 << 6))
#define ULTRADMA_MODE5_SUPPORTED(x)					(x & (1 << 5))
#define ULTRADMA_MODE4_SUPPORTED(x)					(x & (1 << 4))
#define ULTRADMA_MODE3_SUPPORTED(x)					(x & (1 << 3))
#define ULTRADMA_MODE2_SUPPORTED(x)					(x & (1 << 2))
#define ULTRADMA_MODE1_SUPPORTED(x)					(x & (1 << 1))
#define ULTRADMA_MODE0_SUPPORTED(x)					(x & 1)

/*--- DEVICEINFO.bUltraDMASelected ---*/
#define ULTRADMA_MODE6_SELECTED(x)					(x & (1 << 6))
#define ULTRADMA_MODE5_SELECTED(x)					(x & (1 << 5))
#define ULTRADMA_MODE4_SELECTED(x)					(x & (1 << 4))
#define ULTRADMA_MODE3_SELECTED(x)					(x & (1 << 3))
#define ULTRADMA_MODE2_SELECTED(x)					(x & (1 << 2))
#define ULTRADMA_MODE1_SELECTED(x)					(x & (1 << 1))
#define ULTRADMA_MODE0_SELECTED(x)					(x & 1)

/*--- DEVICEINFO.bPIOSupported ---*/
#define PIO_MODE3_SUPPORTED(x)						(x & 1)
#define PIO_MODE4_SUPPORTED(x)						(x & (1 << 1))

/*--- DEVICEINFO.wSATACapabilities ---*/
#define SATA_NCQ_PRIO_INFO_SUPPORTED(x)				(x & (1 << 12))
#define SATA_UNLOAD_NCQ_PENDING_SUPPORTED(x)		(x & (1 << 11))
#define SATA_PHY_EVENT_COUNTERS_SUPPORTED(x)		(x & (1 << 10))
#define SATA_POWER_MGMT_REQUESTS_SUPPORTED(x)		(x & (1 << 9))
#define SATA_NCQ_FEATURES_SUPPORTED(x)				(x & (1 << 8))
#define SATA_GEN2_SIGNAL_SPEED_SUPPORTED(x)			(x & (1 << 2))
#define SATA_GEN1_SIGNAL_SPEED_SUPPORTED(x)			(x & (1 << 1))

/*--- DEVICEINFO.wSATAFeaturesSupported ---*/
#define SATA_SSP_SUPPORTED(x)						(x & (1 << 6))
#define SATA_IN_ORDER_DELIVERY_SUPPORTED(x)			(x & (1 << 4))
#define SATA_INIT_POWER_MGMT_SUPPORTED(x)			(x & (1 << 3))
#define SATA_DMA_SETUP_AUTO_ACT_SUPPORTED(x)		(x & (1 << 2))
#define SATA_NON_ZERO_BUFOFF_SUPPORTED(x)			(x & (1 << 1))

/*--- DEVICEINFO.wSATAFeaturesEnabled ---*/
#define SATA_SSP_ENABLED(x)							(x & (1 << 6))
#define SATA_IN_ORDER_DELIVERY_ENABLED(x)			(x & (1 << 4))
#define SATA_INIT_POWER_MGMT_ENABLED(x)				(x & (1 << 3))
#define SATA_DMA_SETUP_AUTO_ACT_ENABLED(x)			(x & (1 << 2))
#define SATA_NON_ZERO_BUFOFF_ENABLED(x)				(x & (1 << 1))

/*--- DEVICEINFO.brecommendedAAMLevel, DEVICEINFO.bcurrentAAMLevel ---*/
#define AAM_LEVEL_VENDOR_SPECIFIC(x)				(x == 0x00)
#define AAM_LEVEL_MIN_ACOUSTIC(x)					(x == 0x80)
#define AAM_LEVEL_INT_ACOUSTIC(x)					(x >= 0x81 && x <= 0xFD)
#define AAM_LEVEL_MAX_PERFORMANCE(x)				(x == 0xFE)

/*--- DEVICEINFO.bCurrentAPMLevel ---*/
#define APM_LEVEL_MIN_POWER_WITH_STANDBY(x)			(x == 0x01)
#define APM_LEVEL_INT_POWER_WITH_STANDBY(x)			(x >= 0x02 && x <= 0x7F)
#define APM_LEVEL_MIN_POWER_WITHOUT_STANDBY(x)		(x == 0x80)
#define APM_LEVEL_INT_POWER_WITHOUT_STANDBY(x)		(x >= 0x81 && x <= 0xFD)
#define APM_LEVEL_MAX_PERFORMANCE(x)				(x == 0xFE)

/*--- DEVICEINFO.bSCTCommandTransportSupported ---*/
#define SCT_TRANSPORT_SUPPORTED(x)					(x & 1)
#define SCT_DATA_TABLES_SUPPORTED(x)				(x & (1 << 5))
#define SCT_FEATURES_CTRL_SUPPORTED(x)				(x & (1 << 4))
#define SCT_ERROR_RECOVERY_CTRL_SUPPORTED(x)		(x & (1 << 3))
#define SCT_WRITE_SAME_SUPPORTED(x)					(x & (1 << 2))

// structs
typedef struct _DEVICEINFO
{
	// Interface info
	BYTE	bChannelId;
	BYTE	bTargetId;
	BYTE	bLun;
	/// see #define \link #INTERACE_TYPE_UNKNOWN INTERACE_TYPE_* \endlink
	BYTE	bInterfaceType;						

	// Geometry info
	UINT64	qwNumCylinders;
	WORD	wBytesPerSector;
	WORD	wSectorsPerTrack;
	WORD	wTracksPerCylinder;
	/// in Bytes
	WORD	wLogicalSectorSize;					
	/// in Bytes
	WORD	wPhysicalSectorSize;				
	/// in Bytes
	DWORD	dwBufferSize;						
	/// in Bytes
	WORD	wECCSize;							
	/// 0 = unknown, 1 = SSD, other values = rotation rate in RPM
	WORD	wMediaRotationRate;					

	// ATA info
	/// Bitfield: see #define \link #ATA_COMPLIANCE ATA*_SUPPORTED(x) \endlink
	WORD	wATAVersionSupported;				
	/// Bitfield: see #define \link #TRANSPORT_TYPE_AVAILABLE TRANSPORT_TYPE_*(x) \endlink
	WORD	wTransportTypeSupported;			
	UINT64	qwTotalAddressableSectors;

	/// The drive serial number.
	BYTE	bSerialNumber[SERIAL_NUM_LEN];
	/// The drive current firmware revision.
	BYTE	bFirmwareRev[FIRMWARE_REV_LEN];
	/// The drive model number.
	BYTE	bModelNumber[MODEL_NUM_LEN];
	BYTE	bWorldWideID[WWN_LEN];

	// ATA features
	BOOLEAN bSMARTSupported;
	BOOLEAN bSMARTEnabled;
	BOOLEAN bSMARTSelfTestSupported;
	BOOLEAN bSMARTErrorLoggingSupported;

	BOOLEAN bLBASupported;
	BOOLEAN bIORDYSupported;
	BOOLEAN bCFastSupported;

	BOOLEAN bDMASupported;
	/// Bitfield: see #define \link #MULTIDMA_MODE2_SUPPORTED MULTIDMA_MODE*_SUPPORTED(x) \endlink
	BYTE	bMultiwordDMASupported ;			
	/// Bitfield: see #define \link #MULTIDMA_MODE2_SELECTED MULTIDMA_MODE*_SELECTED(x) \endlink
	BYTE	bMultiwordDMASelected ;				

	/// Bitfield: see #define \link #ULTRADMA_MODE6_SUPPORTED ULTRADMA_MODE*_SUPPORTED(x) \endlink
	BYTE	bUltraDMASupported  ;				
	/// Bitfield: see #define \link #ULTRADMA_MODE6_SELECTED ULTRADMA_MODE*_SELECTED(x) \endlink
	BYTE	bUltraDMASelected ;					

	/// Bitfield: see #define \link #PIO_MODE3_SUPPORTED PIO_MODE*_SUPPORTED(x) \endlink
	BYTE	bPIOSupported;						

	BOOLEAN bSATACompliance;
	/// Bitfield: see #define \link #SATA_NCQ_PRIO_INFO_SUPPORTED SATA_*_SUPPORTED(x) \endlink
	WORD	wSATACapabilities;
	/// Bitfield: see #define \link #SATA_SSP_SUPPORTED SATA_*_SUPPORTED(x) \endlink
	WORD	wSATAFeaturesSupported;
	/// Bitfield: see #define \link #SATA_SSP_ENABLED SATA_*_ENABLED(x) \endlink
	WORD	wSATAFeaturesEnabled;
	
	BOOLEAN bTrustedComputingSupported;
	BOOLEAN bHPASupported;
	BOOLEAN bReadLookAheadSupported;
	BOOLEAN bReadLookAheadEnabled;
	BOOLEAN bWriteCacheSupported;
	BOOLEAN bWriteCacheEnabled;
	BOOLEAN bPowerManagementSupported;
	BOOLEAN bSecurityModeSupported;
	BOOLEAN bSecurityModeEnabled;
	BOOLEAN bDeviceConfigOverlaySupported;
	BOOLEAN b48BitAddressSupported;
	BOOLEAN bAutoAcousticSupported;
	BOOLEAN bAutoAcousticEnabled;
	BYTE	bRecommendedAAMLevel;
	BYTE	bCurrentAAMLevel;
	BOOLEAN bPowerUpStandbySupported;
	BOOLEAN bPowerUpStandbyEnabled;
	BOOLEAN bAutoPowerManagementSupported;
	BOOLEAN bAutoPowerManagementEnabled;
	BYTE	bCurrentAPMLevel;
	BOOLEAN bCFASupported;
	BOOLEAN bGPLoggingSupported;
	BOOLEAN bStreamingSupported;
	BOOLEAN bMediaCardPassthroughSupported;
	BOOLEAN bMediaCardPassthroughEnabled;
	BOOLEAN bExtPowerConditionsSupported;
	BOOLEAN bExtPowerConditionsEnabled;
	BOOLEAN bExtStatusReportingSupported;
	BOOLEAN bExtStatusReportingEnabled;
	BOOLEAN bFreefallControlSupported;
	BOOLEAN bFreefallControlEnabled;
	BOOLEAN bWriteReadVerifySupported;
	BOOLEAN bWriteReadVerifyEnabled;
	BOOLEAN bTrimSupported;
	/// Bitfield: see #define \link #SCT_TRANSPORT_SUPPORTED SCT_*_SUPPORTED(x) \endlink
	WORD	wSCTCommandTransportSupported;		
	BOOLEAN	bNVCacheEnabled;
	BOOLEAN	bNVCachePMSupported;
	BOOLEAN	bNVCachePMEnabled;

} DEVICEINFO, *LPDEVICEINFO;

/** @} */ // end of devinfo_group

/*! 
  \brief	 Call this function to initialize usage of the SMART set of APIs.

			 This function should only be called once.  The call enumerates 
			 through all detected drives, initializes the proper structures 
			 and retrieves SMART values. If #SysInfoDll_Smart_Init is not 
			 called first, subsequent SysInfo SMART API calls will fail.

  \note		 #SysInfoDll_Smart_Cleanup must be called afterwards to de-allocate memory.
  \param[in] debug			A flag specifying if debug messages should be logged
  \param[in] iDebugFile		The name of the debug file to write to. If NULL, "SmartDiskDebug.log" will be used.
  \param[in] iRecordTEC		A flag specifying if we want to record and write all SMART attributes to a binary file for the purpose of TEC prediction.  This value defaults to true.
  \return	 #SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_Init( bool debug, 
												   wchar_t *iDebugFile = NULL, 
												   bool iRecordTEC = true );

/*! 
  \brief	Call this function to clean up memory usage of the DLL after 
			finishing all calls to SysInfo SMART APIs. It de-allocates all memory 
			that was allocated and used by SmartDisk DLL.  Calling subsequent 
			SmartDll API calls after SmartDll_Cleanup is an error.

  \remarks	It de-allocates all memory that was allocated by #SysInfoDll_Smart_Init.
  \note		#SysInfoDll_Smart_Cleanup should be called after #SysInfoDll_Smart_Init.
*/
SYSINFO_API void			SysInfoDll_Smart_Cleanup( void );

/*! 
  \brief	 Call this function to refresh (and re-probe) all SMART devices. If iRecordTEC is true, it records S.M.A.R.T. values for all the available S.M.A.R.T. devices discovered.
  \note		 This function should not be called between:
				- #SysInfoDll_Smart_GetSmartDeviceIDFirst
				- #SysInfoDll_Smart_GetSmartDeviceIDNext
				- #SysInfoDll_Smart_GetSmartAttributeIDFirst
				- #SysInfoDll_Smart_GetSmartAttributeIDNext.
  \param[in] iRecordTEC		A flag specifying if we want to record and write all SMART attributes to a binary file for the purpose of TEC prediction.  This value defaults to false.
  \return	 #SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_Refresh( bool iRecordTEC = false );

/*! 
  \brief	 Call this function to set the timeout for retrieving SMART info from a drive.
  \note		 This function must be called before #SysInfoDll_Smart_Init or #SysInfoDll_Smart_Refresh for the timeout to be applicable.
  \param[in] iTimeoutMs		The timeout value in milliseconds
  \return	 #SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_SetInitTimeout( DWORD iTimeoutMs );

/*! 
  \brief	Call this function to return the total number of SMART devices detected. 
  \return	Total number of SMART devices detected or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetNumSmartDevices( void );

/*! 
  \brief	Call this function to return the total number of physical drives detected.
  \return	Total number of drives detected. If there is an error, a negative error code will be returned.
*/
SYSINFO_API int				SysInfoDll_Smart_GetNumDetectedDrives( void );

/*! 
  \brief			Returns the device ID of the first physical device detected.
  \remarks			Zero (0) is a valid and possible device ID.  To check if a device ID is valid, check that it is not negative.
  \return			Device ID of the first discovered physical device or \ref smart_err_group "negative error code".
*/
SYSINFO_API int				SysInfoDll_Smart_GetPhysicalDeviceIDFirst( void );

/*! 
  \brief			Returns the device ID of the next physical device detected.
  \remarks			Zero (0) is a valid and possible device ID.  To check if the device ID is valid, check that it is not negative.
  \note				You must call #SysInfoDll_Smart_GetPhysicalDeviceIDFirst before calling this function.  
					If you call this function before calling #SysInfoDll_Smart_GetPhysicalDeviceIDFirst, 
					outcome is unpredictable.
  \return			Device ID of the next discovered physical device or \ref smart_err_group "negative error code".
*/
SYSINFO_API int				SysInfoDll_Smart_GetPhysicalDeviceIDNext( void );

/*! 
  \brief	 Checks if a particular device ID is a SMART device.
  \param[in] iDeviceID		Device ID to check
  \return	 #SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_IsValidSmartDevice( int iDeviceID );

/*! 
  \brief			Checks if a particular device ID belongs has multiple partitions.

  \remarks			A physical drive can have multiple partitions.  For example:
  
					A: (floppy) - drive number 0<br>
					C: (1st partition on physcial drive 0) - drive number 2<br>
					D: (2nd partition on physcial drive 0) - drive number 3<br>
					E: (cdrom) - drive number 4<br>
		 
					If we call this function and pass in device ID 0, we will get:
		 
					ioDriveNumSameDisk = [2,3],<br>
					ioNumPartition = 2<br>
		 
					Use #SysInfoDll_Smart_GetDriveLetterFromDriveNum to convert the drive number to their corresponding letters.
  \param[out]		ioDriveNumSameDisk		array of integers that will store the drive numbers of all the volumes that shares the same physical disk
  \param[in,out]	ioNumPartition			The size of the ioDriveNumSameDisk array. On return, contains the actual number of items in the array. If buffer is not big enough #SMART_ERR_BUFFER_TOO_SMALL will be returned.
  \param[in]		iDeviceID				Device ID of the hard disk device
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetDevicePartitionInfo( int *ioDriveNumSameDisk , 
																	 int* ioNumPartition ,
																	 int iDeviceID );

/*! 
  \brief			Get the drive letter from the drive number.
  \param[out]		ioDriveLetter	pointer to a character to store the drive letter
  \param[in]		iDriveNum		the drive number of the volume
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetDriveLetterFromDriveNum( wchar_t* ioDriveLetter, 
																		 int iDriveNum );

/*! 
  \brief			Get the device ID from the drive number.
  \remarks			Zero (0) is a valid and possible device ID.  To check if a device ID is valid, check that it is not negative.
  \param[in]		iDriveNum		the drive number of the volume
  \return			Device ID corresponding to the drive number of the volume or \ref smart_err_group "negative error code".
*/
SYSINFO_API int				SysInfoDll_Smart_GetDeviceIDFromDriveNum( int iDriveNum );

/*! 
  \brief			Get the serial number of the device.
  \note				Use a buffer of size #SERIAL_NUM_LEN to store the serial number string.
  \param[out]		ioBuf		pre-allocated character buffer to hold serial number string
  \param[in,out]	ioLen		The size of ioBuf buffer.  On return, contains the serial number string length.
  \param[in]		iDeviceID	Device ID of the hard disk device
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetDeviceSerialNumber( wchar_t* ioBuf ,
																    int* ioLen ,
																    int iDeviceID );

/*! 
  \brief			Get the firmware revision number of the device.
  \note				Use a buffer of size #FIRMWARE_REV_LEN to store the firmware.
  \param[out]		ioBuf		pre-allocated character buffer to hold firmware revision string
  \param[in,out]	ioLen		The size of ioBuf buffer.  On return, contains the firmware revision string length.
  \param[in]		iDeviceID	Device ID of the hard disk device
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetDeviceFirmWareRev( wchar_t* ioBuf ,
																	  int* ioLen , 
																	  int iDeviceID );

/*! 
  \brief			Get the device model number of the device.
  \note				Use a buffer of size #MODEL_NUM_LEN to store the firmware revision string.
  \param[out]		ioBuf		pre-allocated character buffer to hold device model string.
  \param[in,out]	ioLen		The size of ioBuf buffer.  On return, contains the device model string length.
  \param[in]		iDeviceID	Device ID of the hard disk device
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetDeviceModelNumber( wchar_t* ioBuf ,
																   int* ioLen , 
																   int iDeviceID );

/*! 
  \brief			Get the capacity of the device in MB .
  \param[out]		ioCapacity		Pointer to an integer to hold capacity in MB.
  \param[in]		iDeviceID		Device ID of the hard disk device
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetDeviceCapacity( int* ioCapacity ,
															    int iDeviceID );

/*! 
  \brief			Get the capacity of a volume in MB .
  \param[out]		ioCapacity		Pointer to an integer to hold capacity in MB.
  \param[in]		iDriveNum		the drive number of the volume
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetVolumeCapacity( int* ioCapacity ,
															    int iDriveNum );

/*! 
  \brief			Returns the device ID of the first SMART device detected.
  \remarks			Zero (0) is a valid and possible device ID.  To check if a device ID is valid, check that it is not negative.
  \return			Device ID of the first discovered SMART device or \ref smart_err_group "negative error code".
*/
SYSINFO_API int				SysInfoDll_Smart_GetSmartDeviceIDFirst( void );

/*! 
  \brief			Returns the device ID of the next SMART device detected.
  \remarks			Zero (0) is a valid and possible device ID.  To check if the device ID is valid, check that it is not negative.
  \note				You must call #SysInfoDll_Smart_GetSmartDeviceIDFirst before calling this function.  
					If you call this function before calling #SysInfoDll_Smart_GetSmartDeviceIDFirst, 
					outcome is unpredictable.
  \return			Device ID of the next discovered SMART device or \ref smart_err_group "negative error code".
*/
SYSINFO_API int				SysInfoDll_Smart_GetSmartDeviceIDNext( void );

/*! 
  \brief			Returns the number of SMART attributes for a SMART device.
  \param[in]		iDeviceID		Device ID of the SMART device
  \return			The number of SMART attribute of the device or \ref smart_err_group "negative error code".
*/
SYSINFO_API int				SysInfoDll_Smart_GetNumSmartAttr( int iDeviceID );

/*! 
  \brief			Returns the first discovered attribute ID of the SMART device.
  \remarks			Zero(0) is an invalid attribute ID.  To check if attribute is valid, check for value greater than zero.
  \param[in]		iDeviceID	Device ID of the SMART device
  \return			First discovered \ref attr_id_group of the SMART device or \ref smart_err_group "negative error code". If list is empty, #SMART_ERR_SMART_ATTR_ID_EMPTY is returned.
*/
SYSINFO_API int				SysInfoDll_Smart_GetSmartAttributeIDFirst( int iDeviceID );

/*! 
  \brief			Returns the next discovered attribute ID of the SMART device.
  \remarks			Zero(0) is an invalid attribute ID.  To check if attribute is valid, check for value greater than zero.
  \note				You must call #SysInfoDll_Smart_GetSmartAttributeIDFirst before calling this function.
					If you call this function before calling #SysInfoDll_Smart_GetSmartAttributeIDFirst, outcome is unpredictable.
  \param[in]		iDeviceID	Device ID of the SMART device
  \return			Next discovered \ref attr_id_group of the SMART device \ref smart_err_group "negative error code". If we have reached last attribute ID, #SMART_ERR_SMART_ATTR_ID_EOL is returned.
*/
SYSINFO_API int				SysInfoDll_Smart_GetSmartAttributeIDNext( int iDeviceID );

/*! 
  \brief			Refresh the SMART attributes for a SMART device.
  \param[in]		iDeviceID	Device ID of the SMART device
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_RefreshSmartAttributes( int iDeviceID );

/*! 
  \brief			Execute a SMART self-test routine.
  \param[in]		iDeviceID	Device ID of the SMART device
  \param[in]		iTestType	The \link #SMART_SELFTEST self-test\endlink routine to execute.
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_ExecuteSelfTest(int iDeviceID, int iTestType);

/*! 
  \brief			Retrieve the status of a previously executed SMART self-test routine.
  \param[in]		iDeviceID		Device ID of the SMART device
  \param[out]		ioStatus		On return, contains the status code of a previously executed SMART self-test routine.
  \param[out]		ioPercentRem	On return, contains an approximation of the percent of the self-test routine remaining until completion in ten percent increments.
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetSelfTestStatus(int iDeviceID, int* ioStatus, int* ioPercentRem);

/*! 
  \brief			Get the attribute value of a particular attribute ID for a SMART device. 
  \remarks			The values obtained from this function are based on SFF-8035i revision 2 SMART specifications, which has been obsoleted. 
					However, many disk manufacturers still follow the same specifications.
  \param[out]		ioPtr		Pointer to store the requested attribute value. The type of pointer depends on inAttrType:
						- #ATTR_TYPE_INT_STATUS		Pointer to integer (int*). Get the integer value of the attribute’s status (0 if ok, or 1 if threshold exceeded).
						- #ATTR_TYPE_INT_VALUE		Pointer to integer (int*). Get the integer value of the attribute’s normalized current value.
						- #ATTR_TYPE_ULONGLONG_RAW	Pointer to 64bit integer (unsigned long long*). Get the integer value of the attribute’s raw value.
						- #ATTR_TYPE_INT_WORST		Pointer to integer (int*). Get the integer value of the attribute’s normalized worst value.
						- #ATTR_TYPE_INT_THRESHOLD	Pointer to integer (int*). Get the integer value of the attribute’s normalized threshold value.
						- #ATTR_TYPE_STR_DESC		Pointer to wchar_t (wchar_t*). Get string of the attirbute’s description.
  \param[in,out]	ioLen		(#ATTR_TYPE_STR_DESC only) Size of the string buffer (in wchar). On return, contains the length of the description string.
  \param[in]		iAttrID		\ref attr_id_group you want to get
  \param[in]		inAttrType	Attribute type to retrieve(See \link #ATTR_TYPE_STR_DESC ATTR_TYPE_* \endlink)
  \param[in]		iDeviceID	Device ID of the SMART device
  \return			#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetSmartAttributeByID( void* ioPtr , 
																    int* ioLen , 
																    int iAttrID , 
																    int inAttrType , 
																    int iDeviceID );


/*! 
  \brief			Get the attribute description of a particular attribute ID.
  \param[out]		oBuf		A pre-allocated buffer to hold the description string.
  \param[in,out]	ioLen		Size of the string buffer (in wchar).  On return, contains the length of the desc string.
  \param[in]		iAttrID		\ref attr_id_group to retrieve the description for
  \param[in]		iDeviceID	Device ID of the SMART device
  \return #SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetAttrDescByID( wchar_t *oBuf ,
															  int *ioLen , 
															  int iAttrID ,
															  int iDeviceID);


/*! 
  \brief		Get the device information (such as disk geometry, interface, supported/enabled features)
  \param[out]	oDeviceInfo		Pointer to a #DEVICEINFO struct to hold the info.
  \param[in]	iDeviceID		Device ID of hard disk device
  \return		#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetDeviceInfo( DEVICEINFO *oDeviceInfo ,
													    int iDeviceID );

/*! 
  \brief	 Exports an ASCII text report containing the device information, SMART attribute values and history.
  \param[in] filename		string containing the file path to save the report to
  \param[in] iDeviceID		the device ID to generate report for
  \param[in] iHeaderText	string containing additional text to be written at the start of the report.
  \param[in] iHeaderTextLen length of the string in iHeaderText.  If set to zero, iHeaderText shall be ignored.
  \param[in] iExportFlags	flags to indicate which of the following to export:
								- Bit 0 - Device information
								- Bit 1 - SMART attributes
								- Bit 2 - SMART attributes history
  \param[in] iHistoryAttr	\ref attr_id_group to export the history for. Or set to 0 to export the history for all SMART attributes. (ignored if Bit 2 of iExportFlags is not set)
  \return	 #SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_ExportReport( wchar_t *filename ,
														   int iDeviceID , 
														   wchar_t *iHeaderText ,
														   int iHeaderTextLen , 
														   int iExportFlags = 3 ,
														   int iHistoryAttr = 0 );

/*! 
  \brief		Records the current set of normalized and raw attribute values to a binary file for use with TEC prediction.
  \remarks		The created binary file is named "<device serial number>.bin"
  \param[in]	iDeviceID	the device ID to record TEC prediction data for
  \return		#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_RecordTECData( int iDeviceID );

/*! 
  \brief		Get number of TEC prediction data sets recorded for a particular SMART device.
  \remarks		This function checks for a file named "<device serial number>.bin" for TEC data
  \param[out]	ioCount		number of TEC prediction data sets recorded
  \param[in]	iDeviceID	the device ID of the SMART device
  \return		#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetTECDataSetCount( int* ioCount ,
																 int iDeviceID );

/*! 
  \brief		 Extract the TEC prediction data for a particular attribute of a SMART device.
  \remarks		 This function checks for a file named "<device serial number>.bin" for TEC data
  \param[out]	 ioTimeArray		Array of time_t values to store the time of sampling
  \param[out]	 ioNormValArray		Array of integer values to store the normalized values
  \param[out]	 ioWorstValArray	Array of integer values to store the worst values
  \param[in,out] ioArraySize		Pointer to the size of the arrays.  On successful return, contains actual number of sets of TEC values exported.
  \param[in]	 iAttrID			\ref attr_id_group to retrieve the history for
  \param[in]	 iDeviceID			the device ID of the SMART device
  \return		 #SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_ExportTECData( time_t* ioTimeArray , 
														    int* ioNormValArray ,
															int* ioWorstValArray , 
															int* ioArraySize ,
															int iAttrID ,
															int iDeviceID );
/*! 
  \brief		Extract a particular TEC prediction data entry for a particular attribute of a SMART device.
  \remarks		This function checks for a file named "<device serial number>.bin" for TEC data
  \param[out]	ioTime		Pointer to a time_t to store the time of sampling
  \param[out]	ioNormVal	Pointer to an integer to store the normalized value
  \param[out]	ioWorstVal	Pointer to an integer to store the worst value
  \param[in]	inIndex		Index of TEC prediction record to retrieve
  \param[in]	iAttrID		\ref attr_id_group to retrieve the history for
  \param[in]	iDeviceID	the device ID of the SMART device
  \return		#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code"
*/
SYSINFO_API int				SysInfoDll_Smart_GetTECDataAtIndex( time_t* ioTime ,
															    int* ioNormVal , 
																int* ioWorstVal ,
																int inIndex ,
																int iAttrID ,
																int iDeviceID );

/*! 
  \brief		Predicts TEC based on previously recorded TEC prediction data for a particular attribute of a SMART device.
  \remarks		This function checks for a file named "<device serial number>.bin" for TEC data
  \param[out]	ioEarliest	Earliest predicted time for failure
  \param[out]	ioLatest	Latest predicted time for failure
  \param[in]	iAttrID		\ref attr_id_group to use to perform TEC prediction
  \param[in]	iDeviceID	the device ID of the SMART device
  \return		#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code".
  \note			If #SMART_STATUS_SUCCESS is returned, it means ioEarliest is definitely a valid value.  ioLatest is valid only if it is not zero.	
  
				However, note that the following is actually a warning rather than an error:
					- #SMART_WARN_TEC_ATTR_ID_NA: This means that the threshold value corresponding to the attribute ID is zero(0), meaning we have no threshold value to work with to predict TEC.
  
  \attention	Please read our disclaimer statement: \ref smart_disclaimer_anchor "Specific Disclaimer on Failure Predictions".
*/
SYSINFO_API int				SysInfoDll_Smart_EstimateTECByAttributeID( time_t* ioEarliest ,
																	   time_t* ioLatest , 
																	   int iAttrID , 
																	   int iDeviceID );

/*! 
  \brief		Predicts TEC based on previously recorded TEC prediction data for all applicable attributes of a SMART device.
  \remarks		This function checks for a file named "<device serial number>.bin" for TEC data
  \param[out]	ioEarliest	Earliest predicted time for failure
  \param[out]	ioLatest	Latest predicted time for failure
  \param[in]	oAttrID		\ref attr_id_group used to perform TEC prediction
  \param[in]	iDeviceID	the device ID of the SMART device
  \return		#SMART_STATUS_SUCCESS (0) if no error or \ref smart_err_group "negative error code".
  \note			If #SMART_STATUS_SUCCESS is returned, it means ioEarliest is definitely a valid value.  ioLatest is valid only if it isnot zero.

				However, note that the following are actually warnings rather than errors:
					- #SMART_WARN_TEC_NOT_ENOUGH_DATA: This means we do not have at minimum of 3 sets of TEC data to estimate TEC.
					- #SMART_WARN_TEC_UNABLE_TO_PREDICT: This means that that the TEC data recorded does not show any variance (i.e. they remain constant).  Hence, we cannot predict TEC.
  
  \attention	Please read our disclaimer statement: \ref smart_disclaimer_anchor "Specific Disclaimer on Failure Predictions".
*/
SYSINFO_API int				SysInfoDll_Smart_EstimateTEC( time_t* ioEarliest ,
														  time_t* ioLatest ,
														  int* oAttrID ,
														  int iDeviceID );

/** @} */ // end of smart_group


/*=======================================================================
SysInfo DLL Temperature APIs

   The Temperature APIs is a self-contained subcomponent of the SysInfo DLL
   for retrieving temperature information from supported CPUs, GPUs and hard 
   disks.
=======================================================================*/

/** @defgroup temperature_group TEMPERATURE APIs
 *  @{
 */

// defines
/*--- General --- */
///Flags that no data was available for the sample period (e.g. HDD failed)
#define NOVALUEAVAILABLE   -1001.0f		

///Number of temperature samples to store in the sensor data history
#define NUMTEMPSAMPLES		  1000		
///The maximum number of possible temperature sources, note that this is all temperature sources - not just the configured sources
#define MAXNUMTEMPVALUES	   200		
///The maximum number of temperature sources that can be selected for monitoring 
#define MAXNUMTEMPSOURCES		 6		

///Minimum disk temperature range allowed ( > 1 and < 128). This is due to some SSD's indicating 1C and 128C, but not supporting teperature sensors						
#define TEMP_MIN_DISK 1					
///Maximum disk temperature range allowed ( > 1 and < 128). This is due to some SSD's indicating 1C and 128C, but not supporting teperature sensors						
#define TEMP_MAX_DISK 128				

///Reserved for future use
#define MAXNUMFANVALUES			5		
///Reserved for future use
#define MAXVOLTAGEVALUES		7		

///The type of temperature source - bit coded
enum PM_Temp_Type
{
	PM_TEMP_NONE = 0,
	PM_TEMP_CPU_AVE = 1,	//Average CPU package temerature (based on Intel individual core temperatures)
	PM_TEMP_CPU = 2,		//CPU temp
	PM_TEMP_CPU_CORE = 4,	//Intel temp per core
	PM_TEMP_HDD = 8,		//Disk drive SMART temperature
	PM_TEMP_GPU = 16,		//Graphics card temperature
	PM_TEMP_SENSOR = 32,	//Unknown what hte temperature source is (e.g. from 3rd party application with no source type in interface)
};

enum PM_Auto_Type
{
	PM_TEMP_AUTO_ALL = 0,
	PM_TEMP_AUTO_CPU,
	PM_TEMP_AUTO_HDD,
	PM_TEMP_AUTO_GPU,
};

///The raw temperature infomration from the CPU registers
typedef struct _MSR_TEMP
{
	bool		bDataCollected;
	DWORD_PTR	pdwCPUAffinityMask;				//CPU id bit coded 1,2,4,8...
	ULONGLONG	ullMSR0x19C;					//Raw register
	int			iPROCHOT1;
	int			iPROCHOT2;
	int			iResolutionInDegreesCelcius;
	int			iReadingValid;
	int			iDigitalReadout;				//CPU temp value (yet to be adjusted, e.g. for tJunction)
	int			iTemperature;					//Calculated actual temperature
} MSR_TEMP;

////drive_type
#define DRIVE_TYPE_UNKNOWN			 -1
#define DRIVE_TYPE_FIXED			  3
#define DRIVE_TYPE_FIXED_HOTPLUG	100

typedef struct _PM_TEMP_KEY
{
    PM_Temp_Type	iType;			// type of temperature value
    int				iID;			// e.g. CPU0, CPU1 (pysical package), HDD0, HDD1, GPU0, GPU1 etc
    int				iIDSubvalue;	// e.g. Core 0,1,2, or 3, if HDD then drivetype 
	INT64			iCPUMask;		// e.g. CPU affinity mask for this CPU/Core		//BIT7000000006 - win32/64 different
} PM_TEMP_KEY;

///Structure to hold temperature sensor data
typedef struct  TSensorsdata 
{
    PM_TEMP_KEY	iKey[MAXNUMTEMPVALUES];								// type of temperature value
    int			iPMKeySelected[MAXNUMTEMPVALUES];					// 0 if if this PassMark temperature source has not been selected by the user, otherwise the index into testconfig.PMTempSource
    TCHAR		szTempCaption[MAXNUMTEMPVALUES][SHORT_STRING_LEN];	// temperatures, Normally CPU1, CPU2, etc
    float		Temp[MAXNUMTEMPVALUES];								// temperatures, Normally CPU1, CPU2, etc
    float		MaxTemp[MAXNUMTEMPVALUES];							// Max temperatures
    float		MinTemp[MAXNUMTEMPVALUES];							// Min temperatures
    TCHAR		szVoltageCaption[MAXVOLTAGEVALUES][SHORT_STRING_LEN];	// Reserved for future use
    float		VIO,VCORE,VCORE2;									// Reserved for future use
    float		VP5,VP12,VN12,VN5;									// Reserved for future use
    TCHAR		szFanCaption[MAXNUMFANVALUES][SHORT_STRING_LEN];	// Reserved for future use
	int			FAN1,FAN2,FAN3,FAN4,FAN5;							// Reserved for future use
	MSR_TEMP	CPUMSRTemp[MAXNUMTEMPVALUES];						// Raw CPU temp information
	int			iTempIndex;											//Current index into Temperature samples index
	bool		bTempHistoryWrap;									//True once the max number of samples has been taken and the TempHistory wraps
	SYSTEMTIME	TempHistorySampleTime[NUMTEMPSAMPLES];				//Temperature sample times. It is assumed that all temperature sources are sampled at the same time
	float		flTempHistory[MAXNUMTEMPVALUES][NUMTEMPSAMPLES];	//Temperature history for each temperature source, NUMTEMPSAMPLES samples

} TSENSORSDATA;

/*! 
  \brief		Prepare for temperature monitoring by determining what temperature sensors are avalable:
				Initialize the temperature sensor data.
				Check if temperature monitoring is supported by Intel CPUs and if so save information about what CPU temperature sensors are available
				Check if temperature monitoring is supported by AMD CPUs and if so save information about what CPU temperature sensors are available
				Check which disk drives support SMART temperature reporing and save information about what disk drive temperature sensors are available
				Check if temperature monitoring is supported by nVIDIA GPU and if so save information about what GPU temperature sensors are available
				Check if temperature monitoring is supported by AMD GPU and if so save information about what GPU temperature sensors are available

				Note: This API can take quite some time to complete. Especially if initialized for disk drives. As such, it may make sense to thread this API call.

  \param[in]	dwTempTypes: Specifies which temperature sources to gather temperature sensor support e.g. bitwise (PM_TEMP_CPU |PM_TEMP_HDD | PM_TEMP_GPU)
  \param[in]	pCPUinfo: The system CPU information obtained using SysInfoDll_GetCPUInfo()
  \param[in]	WMI_HDDinfo: The system physcial disk drive information obtained using SysInfoDll_GetHDDetails(), SysInfoDll_GetSystemInfo_WMI() or 
							 SysInfoDll_GetSystemInfo_WMI_async ();
  \param[in]	pbSMARTInitialized: Pointer to a boolean, true if SysInfoDll_Smart_Init() has already been called, false otherwise. 
								    Note: The calling application may have already initialzed the SysInfo SMART functions using, SysInfoDll_Smart_Init(), for some other purpose.
  \param[out]	psdata: The sensor data including the temperature sources
  \returns		True if successfully initialized
*/
SYSINFO_API bool SysInfoDll_InitPassMarkTemp(DWORD dwTempTypes, CPUINFO *pCPUinfo, SYSINFO_WMI *WMI_HDDinfo, bool *pbSMARTInitialized, TSENSORSDATA * psdata);

/*! 
  \brief		Gets the temperatures and stores the results:
				Get all the available CPU temperatures, saves them (in Temp, MinTemp and MaxTemp) and as a timestamped data points (in TempHistorySampleTime[])
				Get all the available disk drive temperatures, saves them (in Temp, MinTemp and MaxTemp) and as a timestamped data points (in TempHistorySampleTime[])
				Get all the available GPU temperatures, saves them (in Temp, MinTemp and MaxTemp) and as a timestamped data points (in TempHistorySampleTime[])
  \param[in]	dwTempTypes: Specifies which temperature sources to gather temperature sensor support e.g. bitwise (PM_TEMP_CPU |PM_TEMP_HDD | PM_TEMP_GPU)
  \param[in]	hDirectIO: To collect CPU temperature the PassMark DirectIO device driver must be loaded with SysInfoDll_InitDirectIO(). SysInfoDll_InitDirectIO
						   should be called once in your applciation to load the device driver and get the handle.
  \param[in]	sztemp_C_or_F: L'C' for Celcius or L'F' for Fahrenheit
  \param[in]	pCPUinfo: The system CPU information obtained using SysInfoDll_GetCPUInfo()
  \param[out]	psdata: The sensor data including the temperature data
  \returns		True if any temperature data is available
*/
SYSINFO_API bool SysInfoDll_GetPassMarkTemps(DWORD dwTempTypes, HANDLE hDirectIO, wchar_t sztemp_C_or_F, CPUINFO *pCPUinfo, TSENSORSDATA * psdata);

/** @} */ // end of temperature_group


/*=======================================================================
SysInfo DLL Installed Software APIs

   The Installed Software APIs is a self-contained subcomponent of the 
   SysInfo DLL for retrieving the software information on the system and 
   the user name it is installed under.
=======================================================================*/

/** @defgroup software_group SOFTWARE APIs
 *  @{
 */

// defines
#define MAX_SOFTWARE_RECORDS	1000
#define MAX_SOFTWARE_NAME_LEN	 128
#define MAX_USER_NAME_LEN		  64

typedef struct _SOFTWARE_RECORD {
	wchar_t		SoftwareName[MAX_SOFTWARE_NAME_LEN];	//Description/name of installed program
	wchar_t		Path[MAX_PATH];							//Instalaltion directy
}SOFTWARE_RECORD, *PSOFTWARE_RECORD;

typedef struct _SOFTWARE_LIST {
	int				iCount;								//Number of entries
	SOFTWARE_RECORD	*sw[MAX_SOFTWARE_RECORDS];			//Array of points to SOFTWARE_RECORDs
}SOFTWARE_LIST, *PSOFTWARE_LIST;


/*! 
  \brief		Gets an array of all software installed on the system.
  \param[in]	swl: pointer to a software list structure. 
  \returns		Number of software packages installed
*/
int SysInfoDll_GetInstalledProgramsSystem(SOFTWARE_LIST *swl);

/*! 
  \brief		Gets an array of the software installed on the system for the current user.
  \param[in]	swl: pointer to a software list structure. 
  \returns		Number of software packages installed
*/
int SysInfoDll_GetInstalledProgramsUser(SOFTWARE_LIST *swl);


/*! 
  \brief		Release any memory allcoated by SysInfoDll_GetInstalledProgramsSystem() or SysInfoDll_GetInstalledProgramsUser().
  \param[in]	swl: pointer to a software list structure. 
  \returns		Number of strings
*/
void SysInfoDll_GetInstalledPrograms_ReleaseMem(SOFTWARE_LIST *swl);

/** @} */ // end of software_group

#endif