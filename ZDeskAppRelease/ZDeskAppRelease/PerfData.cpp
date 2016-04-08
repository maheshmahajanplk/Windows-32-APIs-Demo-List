     
 #define WIN32_LEAN_AND_MEAN    /* Exclude rarely-used stuff from Windows headers */
 #include "stdafx.h"


 #include <windows.h>
 #include <commctrl.h>
 #include <stdlib.h>
 #include <malloc.h>
 #include <memory.h>
 #include <tchar.h>
 #include <stdio.h>
 #include <winnt.h>
// #include "global.h"
 #include "perfdata.h"
 

extern char		g_szLogDirectory[MAX_PATH + 100];

 PROCNTQSI							g_NtQuerySystemInformation	=	NULL;
 PROCGGR                            g_pGetGuiResources			=	NULL;
 PROCGPIC							g_pGetProcessIoCounters		=	NULL;
 PPERFDATA							g_pPerfDataOld				=	NULL;    /* Older perf data (saved to establish delta values) */
 PPERFDATA							g_pPerfData					=	NULL;    /* Most recent copy of perf data */
 ULONG								g_ProcessCountOld			=	0;
 ULONG								g_ProcessCount				=	0;
 double								g_dbIdleTime				=	0;
 double								g_dbKernelTime				=	0;
 double								g_dbSystemTime				=	0;
 LARGE_INTEGER						g_liOldIdleTime				=	{{0,0}};
 double								g_OldKernelTime				=	0;
 LARGE_INTEGER						g_liOldSystemTime			=	{{0,0}};
 SYSTEM_PERFORMANCE_INFORMATION		g_SystemPerfInfo			=	{0};
 SYSTEM_BASIC_INFORMATION			g_SystemBasicInfo			=	{0};
 SYSTEM_CACHE_INFORMATION			g_SystemCacheInfo			=	{0};
 SYSTEM_HANDLE_INFORMATION			g_SystemHandleInfo			=	{0};
 PSYSTEM_PROCESSORTIME_INFO			g_SystemProcessorTimeInfo	=	NULL;
 

 BOOL PerfDataInitialize(void)
 {
	 try
	 {
		LONG    status = 0;

		g_ProcessCountOld			=	0;
		g_ProcessCount				=	0;
 		g_dbIdleTime				=	0;
 		g_dbKernelTime				=	0;
 		g_dbSystemTime				=	0;
 		//g_liOldIdleTime				=	{{0,0}};
		g_OldKernelTime				=	0;
 		//g_liOldSystemTime			=	{{0,0}};


		if (g_SystemProcessorTimeInfo) 
		{
			free(g_SystemProcessorTimeInfo);
			g_SystemProcessorTimeInfo = NULL;
		}
		if (g_pPerfDataOld) 
		{
			free(g_pPerfDataOld);
			g_pPerfDataOld = NULL;
		}
		if (g_pPerfData) 
		{
			free(g_pPerfData);
			g_pPerfData = NULL;
		}
	 
		g_NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtQuerySystemInformation");
		//g_pGetGuiResources = (PROCGGR)GetProcAddress(GetModuleHandle(_T("user32.dll")), "GetGuiResources");
		//g_pGetProcessIoCounters = (PROCGPIC)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetProcessIoCounters");
	     
	    
		if (!g_NtQuerySystemInformation)
			return FALSE;
	     

		/*
		* Get number of processors in the system
		*/
		status = g_NtQuerySystemInformation(SystemBasicInformation, &g_SystemBasicInfo, sizeof(g_SystemBasicInfo), NULL);
		if (status != NO_ERROR)
			return FALSE;
	 }
	 catch(...)
	 {
		return FALSE;
	 }
     
     return TRUE;
 }
 
 void PerfDataUninitialize(void)
 {
	 try
	 {
		g_NtQuerySystemInformation = NULL;
		if (g_SystemProcessorTimeInfo) 
		{
			free(g_SystemProcessorTimeInfo);
			g_SystemProcessorTimeInfo = NULL;
		}
		if (g_pPerfDataOld) 
		{
			free(g_pPerfDataOld);
			g_pPerfDataOld = NULL;
		}
		if (g_pPerfData) 
		{
			free(g_pPerfData);
			g_pPerfData = NULL;
		}
	 }
	 catch(...)
	 {

	 }
 }
 
 void PerfDataRefresh(void)
 {
	 try
	 {
		ULONG								ulSize						=	0;
		LONG								status						=	0;
		LPBYTE								pBuffer						=	0;
		ULONG								BufferSize					=	0;
		PSYSTEM_PROCESS_INFORMATION         pSPI						=	0;
		PPERFDATA							pPDOld						=	0;
		ULONG								Idx							=	0;
		ULONG								Idx2						=	0;
		HANDLE								hProcess					=	0;
		HANDLE								hProcessToken				=	0;
		TCHAR								szTemp[MAX_PATH + 100]		=	{0};
		DWORD								dwSize						=	0;
		SYSTEM_PERFORMANCE_INFORMATION		SysPerfInfo					=	{0};
		SYSTEM_TIME_INFORMATION             SysTimeInfo					=	{0};
		SYSTEM_CACHE_INFORMATION			SysCacheInfo				=	{0};
		LPBYTE								SysHandleInfoData			=	0;
		PSYSTEM_PROCESSORTIME_INFO			SysProcessorTimeInfo		=	0;
		double								CurrentKernelTime			=	0;
		int									iControlLoop1				=	0;
		int									iControlLoop2				=	0;
		int									iRequiredMem				=	(64 * 1024);
	 
	 

		if (!g_NtQuerySystemInformation)
			return;

	 
		/* Get new system time */
		status = g_NtQuerySystemInformation(SystemTimeInformation, &SysTimeInfo, sizeof(SysTimeInfo), 0);
		if (status != NO_ERROR)
			return;
	 
		/* Get new CPU's idle time */
		status = g_NtQuerySystemInformation(SystemPerformanceInformation, &SysPerfInfo, sizeof(SysPerfInfo), NULL);
		if (status != NO_ERROR)
			return;
	 
		/* Get system cache information */
		status = g_NtQuerySystemInformation(SystemCacheInformation, &SysCacheInfo, sizeof(SysCacheInfo), NULL);
		if (status != NO_ERROR)
			return;
	 
		/* Get processor time information */
		SysProcessorTimeInfo = (PSYSTEM_PROCESSORTIME_INFO)malloc(sizeof(SYSTEM_PROCESSORTIME_INFO) * g_SystemBasicInfo.bKeNumberProcessors);
		status = g_NtQuerySystemInformation(SystemProcessorTimeInformation, SysProcessorTimeInfo, sizeof(SYSTEM_PROCESSORTIME_INFO) * g_SystemBasicInfo.bKeNumberProcessors, &ulSize);
		if (status != NO_ERROR)
			return;


		/* Get handle information
		* We don't know how much data there is so just keep
		* increasing the buffer size until the call succeeds
		*/
		BufferSize = 0;
		iControlLoop1 = 0;
		iRequiredMem = (64 * 1024);
		do
		{
			++iControlLoop1;
			if(iControlLoop1 > 10)
			{
				return;
			}
			BufferSize += iRequiredMem;
			SysHandleInfoData = (LPBYTE)malloc(BufferSize);

			if(NULL == SysHandleInfoData)
			{
				return;
			}
	 
			ulSize = 0;
			status = g_NtQuerySystemInformation(SystemHandleInformation, SysHandleInfoData, BufferSize, &ulSize);
	 
			if (status == 0xC0000004 /*STATUS_INFO_LENGTH_MISMATCH*/)
			{
				if(ulSize > BufferSize)
				{
					BufferSize = ulSize;
					iRequiredMem = (iRequiredMem / 2);
				}
				free(SysHandleInfoData);
				SysHandleInfoData = NULL;
			}
		} while (status == 0xC0000004 /*STATUS_INFO_LENGTH_MISMATCH*/);
	 


		/* Get process information
		* We don't know how much data there is so just keep
		* increasing the buffer size until the call succeeds
		*/
		BufferSize = 0;
		iControlLoop1 = 0;
		iRequiredMem = (64 * 1024);
		do
		{
			++iControlLoop1;
			if(iControlLoop1 > 10)
			{
				return;
			}
			BufferSize += iRequiredMem;
			pBuffer = (LPBYTE)malloc(BufferSize);
	 
			status = g_NtQuerySystemInformation(SystemProcessInformation, pBuffer, BufferSize, &ulSize);
	 
			if (status == 0xC0000004 /*STATUS_INFO_LENGTH_MISMATCH*/) 
			{
				free(pBuffer);
				pBuffer = NULL;
				if(ulSize > BufferSize)
				{
					BufferSize = ulSize;
					iRequiredMem = (iRequiredMem / 2);
				}
			}
	 
		} while (status == 0xC0000004 /*STATUS_INFO_LENGTH_MISMATCH*/);
	 
	 	
		/*
		* Save system performance info
		*/
		memcpy(&g_SystemPerfInfo, &SysPerfInfo, sizeof(SYSTEM_PERFORMANCE_INFORMATION));
	 
		/*
		* Save system cache info
		*/
		memcpy(&g_SystemCacheInfo, &SysCacheInfo, sizeof(SYSTEM_CACHE_INFORMATION));
	     
		/*
		* Save system processor time info
		*/
		if (g_SystemProcessorTimeInfo) 
		{
			free(g_SystemProcessorTimeInfo);
			g_SystemProcessorTimeInfo = NULL;
		}
		g_SystemProcessorTimeInfo = SysProcessorTimeInfo;
		SysProcessorTimeInfo = NULL;
	     
		/*
		* Save system handle info
		*/
		memcpy(&g_SystemHandleInfo, SysHandleInfoData, sizeof(SYSTEM_HANDLE_INFORMATION));
		free(SysHandleInfoData);
		SysHandleInfoData = NULL;
		


	     
		for (CurrentKernelTime=0, Idx=0; Idx<g_SystemBasicInfo.bKeNumberProcessors; Idx++) 
		{
			CurrentKernelTime += Li2Double(g_SystemProcessorTimeInfo[Idx].KernelTime);
			CurrentKernelTime += Li2Double(g_SystemProcessorTimeInfo[Idx].DpcTime);
			CurrentKernelTime += Li2Double(g_SystemProcessorTimeInfo[Idx].InterruptTime);
		}


		// If it's a first call - skip idle time calcs 
		if (g_liOldIdleTime.QuadPart != 0) 
		{
			//  CurrentValue = NewValue - OldValue 
			g_dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(g_liOldIdleTime);
			g_dbKernelTime = CurrentKernelTime - g_OldKernelTime;
			g_dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(g_liOldSystemTime);
	 
			//  CurrentCpuIdle = IdleTime / SystemTime 
			g_dbIdleTime = g_dbIdleTime / g_dbSystemTime;
			g_dbKernelTime = g_dbKernelTime / g_dbSystemTime;
	         
			//  CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors 
			g_dbIdleTime = 100.0 - g_dbIdleTime * 100.0 / (double)g_SystemBasicInfo.bKeNumberProcessors; 
			g_dbKernelTime = 100.0 - g_dbKernelTime * 100.0 / (double)g_SystemBasicInfo.bKeNumberProcessors; 
		}
	 
		// Store new CPU's idle and system time 
		g_liOldIdleTime		= SysPerfInfo.liIdleTime;
		g_liOldSystemTime	= SysTimeInfo.liKeSystemTime;
		g_OldKernelTime		= CurrentKernelTime;
	 
		// Determine the process count
		//We loop through the data we got from g_NtQuerySystemInformation
		//and count how many structures there are (until RelativeOffset is 0)
		
		g_ProcessCountOld = g_ProcessCount;
		g_ProcessCount = 0;
		pSPI = (PSYSTEM_PROCESS_INFORMATION)pBuffer;

		while (pSPI) 
		{
			g_ProcessCount++;
			if (pSPI->RelativeOffset == 0)
				break;
			pSPI = (PSYSTEM_PROCESS_INFORMATION)((LPBYTE)pSPI + pSPI->RelativeOffset);
		}
	 


		// Now alloc a new PERFDATA array and fill in the data 
		if (g_pPerfDataOld) 
		{
			free(g_pPerfDataOld);
			g_pPerfDataOld = NULL;
		}
		g_pPerfDataOld = g_pPerfData;
		g_pPerfData = (PPERFDATA)malloc(sizeof(PERFDATA) * g_ProcessCount);
		pSPI = (PSYSTEM_PROCESS_INFORMATION)pBuffer;
		for (Idx=0; Idx<g_ProcessCount; Idx++) 
		{
			// Get the old perf data for this process (if any) 
			// so that we can establish delta values 
			pPDOld = NULL;
			for (Idx2=0; Idx2<g_ProcessCountOld; Idx2++) 
			{
				if (g_pPerfDataOld[Idx2].ProcessId == pSPI->ProcessId) 
				{
					pPDOld = &g_pPerfDataOld[Idx2];
					break;
				}
			}
	 
			// Clear out process perf data structure 
			memset(&g_pPerfData[Idx], 0, sizeof(PERFDATA));
	 
			if (pSPI->Name.Buffer)
			{
				lstrcpyW(g_pPerfData[Idx].ImageName, pSPI->Name.Buffer);
			}
			else
			{
				static const WCHAR idleW[] = {'S','y','s','t','e','m',' ','I','d','l','e',' ','P','r','o','c','e','s','s',0};
				lstrcpyW(g_pPerfData[Idx].ImageName, idleW );
			}
	 
			g_pPerfData[Idx].ProcessId = pSPI->ProcessId;
	 
			if (pPDOld)    
			{
				double    CurTime = Li2Double(pSPI->KernelTime) + Li2Double(pSPI->UserTime);
				double    OldTime = Li2Double(pPDOld->KernelTime) + Li2Double(pPDOld->UserTime);
				double    CpuTime = (CurTime - OldTime) / g_dbSystemTime;
				CpuTime = CpuTime * 100.0 / (double)g_SystemBasicInfo.bKeNumberProcessors; 
				if(CpuTime > 100)
				{
					CpuTime = 100;
				}
				g_pPerfData[Idx].CPUUsage = (ULONG)CpuTime;
			}
			g_pPerfData[Idx].CPUTime.QuadPart		= pSPI->UserTime.QuadPart + pSPI->KernelTime.QuadPart;
			g_pPerfData[Idx].WorkingSetSizeBytes		= pSPI->TotalWorkingSetSizeBytes;
			g_pPerfData[Idx].PeakWorkingSetSizeBytes = pSPI->PeakWorkingSetSizeBytes;
			if (pPDOld)
			{
				g_pPerfData[Idx].WorkingSetSizeDelta = labs((LONG)pSPI->TotalWorkingSetSizeBytes - (LONG)pPDOld->WorkingSetSizeBytes);
			}
			else
			{
				g_pPerfData[Idx].WorkingSetSizeDelta = 0;
			}
			g_pPerfData[Idx].PageFaultCount = pSPI->PageFaultCount;
			if (pPDOld)
			{
				g_pPerfData[Idx].PageFaultCountDelta = labs((LONG)pSPI->PageFaultCount - (LONG)pPDOld->PageFaultCount);
			}
			else
			{
				g_pPerfData[Idx].PageFaultCountDelta = 0;
			}
			g_pPerfData[Idx].VirtualMemorySizeBytes	= pSPI->TotalVirtualSizeBytes;
			g_pPerfData[Idx].PagedPoolUsagePages		= pSPI->TotalPagedPoolUsagePages;
			g_pPerfData[Idx].NonPagedPoolUsagePages	= pSPI->TotalNonPagedPoolUsagePages;
			g_pPerfData[Idx].BasePriority			= pSPI->BasePriority;
			g_pPerfData[Idx].HandleCount				= pSPI->HandleCount;
			g_pPerfData[Idx].ThreadCount				= pSPI->ThreadCount;
			g_pPerfData[Idx].SessionId				= pSPI->SessionId;

			g_pPerfData[Idx].UserTime.QuadPart	= pSPI->UserTime.QuadPart;
			g_pPerfData[Idx].KernelTime.QuadPart = pSPI->KernelTime.QuadPart;
			pSPI = (PSYSTEM_PROCESS_INFORMATION)((LPBYTE)pSPI + pSPI->RelativeOffset);
		}
		
		free(pBuffer);
		pBuffer = NULL;

		if (g_SystemProcessorTimeInfo) 
		{
			free(g_SystemProcessorTimeInfo);
			g_SystemProcessorTimeInfo = NULL;
		}

	 }
	 catch(...)
	 {

	 }
 }
 
 ULONG PerfDataGetProcessCount(void)
 {
     return g_ProcessCount;
 }
 
 ULONG PerfDataGetProcessorUsage(void)
 {
     return (ULONG)g_dbIdleTime;
 }
 
 ULONG PerfDataGetProcessorSystemUsage(void)
 {
     return (ULONG)g_dbKernelTime;
 }
 
 BOOL PerfDataGetImageName(ULONG Index, LPTSTR lpImageName, int nMaxCount)
 {
	BOOL    bSuccessful = FALSE;
	 try
	 {
		if (Index < g_ProcessCount) {
	#ifdef _UNICODE
				wcsncpy(lpImageName, g_pPerfData[Index].ImageName, nMaxCount);
	#else
				WideCharToMultiByte(CP_ACP, 0, g_pPerfData[Index].ImageName, -1, lpImageName, nMaxCount, NULL, NULL);
	#endif
	 
			bSuccessful = TRUE;
		} else {
			bSuccessful = FALSE;
		}
	 }
	 catch(...)
	 {

	 }
     return bSuccessful;
 }
 
 ULONG PerfDataGetProcessId(ULONG Index)
 {
	 ULONG    ProcessId = 0;
	 try
	 {
	  
		if (Index < g_ProcessCount)
			ProcessId = g_pPerfData[Index].ProcessId;
		else
			ProcessId = 0;
	 }
	 catch(...)
	 {

	 }
     return ProcessId;
 }
 
 BOOL PerfDataGetUserName(ULONG Index, LPTSTR lpUserName, int nMaxCount)
 {
     BOOL    bSuccessful = FALSE;
 
	 try
	 {
		if (Index < g_ProcessCount) {
	#ifdef _UNICODE
				wcsncpy(lpUserName, g_pPerfData[Index].UserName, nMaxCount);
	#else
				WideCharToMultiByte(CP_ACP, 0, g_pPerfData[Index].UserName, -1, lpUserName, nMaxCount, NULL, NULL);
	#endif
	 
			bSuccessful = TRUE;
		} else {
			bSuccessful = FALSE;
		}
	 }
	 catch(...)
	 {

	 }
     return bSuccessful;
 }
 
 ULONG PerfDataGetSessionId(ULONG Index)
 {
     ULONG    SessionId = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			SessionId = g_pPerfData[Index].SessionId;
		else
			SessionId = 0;
	 }
	 catch(...)
	 {

	 }
     return SessionId;
 }
 
 ULONG PerfDataGetCPUUsage(ULONG Index)
 {
     ULONG    CpuUsage = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			CpuUsage = g_pPerfData[Index].CPUUsage;
		else
			CpuUsage = 0;
	 }
	 catch(...)
	 {

	 }
     return CpuUsage;
 }
 
 TIME PerfDataGetCPUTime(ULONG Index)
 {
     TIME    CpuTime = {{0,0}};
	 try
	 {
		if (Index < g_ProcessCount)
			CpuTime = g_pPerfData[Index].CPUTime;
	 }
	 catch(...)
	 {

	 }
     return CpuTime;
 }
 
 ULONG PerfDataGetWorkingSetSizeBytes(ULONG Index)
 {
     ULONG    WorkingSetSizeBytes = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			WorkingSetSizeBytes = g_pPerfData[Index].WorkingSetSizeBytes;
		else
			WorkingSetSizeBytes = 0;
	 }
	 catch(...)
	 {

	 }
     return WorkingSetSizeBytes;
 }
 
 ULONG PerfDataGetPeakWorkingSetSizeBytes(ULONG Index)
 {
     ULONG    PeakWorkingSetSizeBytes = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			PeakWorkingSetSizeBytes = g_pPerfData[Index].PeakWorkingSetSizeBytes;
		else
			PeakWorkingSetSizeBytes = 0;
	 }
	 catch(...)
	 {

	 }
     return PeakWorkingSetSizeBytes;
 }
 
 ULONG PerfDataGetWorkingSetSizeDelta(ULONG Index)
 {
     ULONG    WorkingSetSizeDelta = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			WorkingSetSizeDelta = g_pPerfData[Index].WorkingSetSizeDelta;
		else
			WorkingSetSizeDelta = 0;
	 }
	 catch(...)
	 {

	 }
     return WorkingSetSizeDelta;
 }
 
 ULONG PerfDataGetPageFaultCount(ULONG Index)
 {
     ULONG    PageFaultCount	=	0;
	 try
	 {
		if (Index < g_ProcessCount)
			PageFaultCount = g_pPerfData[Index].PageFaultCount;
		else
			PageFaultCount = 0;
	 }
	 catch(...)
	 {

	 }
     return PageFaultCount;
 }
 
 ULONG PerfDataGetPageFaultCountDelta(ULONG Index)
 {
     ULONG    PageFaultCountDelta = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			PageFaultCountDelta = g_pPerfData[Index].PageFaultCountDelta;
		else
			PageFaultCountDelta = 0;
	 }
	 catch(...)
	 {

	 }
     return PageFaultCountDelta;
 }
 
 ULONG PerfDataGetVirtualMemorySizeBytes(ULONG Index)
 {
     ULONG    VirtualMemorySizeBytes = 0;
	 try
	 {
	 
		if (Index < g_ProcessCount)
			VirtualMemorySizeBytes = g_pPerfData[Index].VirtualMemorySizeBytes;
		else
			VirtualMemorySizeBytes = 0;
	 }
	 catch(...)
	 {

	 }
     return VirtualMemorySizeBytes;
 }
 
 ULONG PerfDataGetPagedPoolUsagePages(ULONG Index)
 {
     ULONG    PagedPoolUsagePages = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			PagedPoolUsagePages = g_pPerfData[Index].PagedPoolUsagePages;
		else
			PagedPoolUsagePages = 0;
	 }
	 catch(...)
	 {

	 }
       
     return PagedPoolUsagePages;
 }
 
 ULONG PerfDataGetNonPagedPoolUsagePages(ULONG Index)
 {
     ULONG    NonPagedPoolUsagePages = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			NonPagedPoolUsagePages = g_pPerfData[Index].NonPagedPoolUsagePages;
		else
			NonPagedPoolUsagePages = 0;
	 }
	 catch(...)
	 {

	 }
     return NonPagedPoolUsagePages;
 }
 
 ULONG PerfDataGetBasePriority(ULONG Index)
 {
     ULONG    BasePriority = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			BasePriority = g_pPerfData[Index].BasePriority;
		else
			BasePriority = 0;
	 }
	 catch(...)
	 {

	 }
     return BasePriority;
 }
 
 ULONG PerfDataGetHandleCount(ULONG Index)
 {
     ULONG    HandleCount = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			HandleCount = g_pPerfData[Index].HandleCount;
		else
			HandleCount = 0;
	 }
	 catch(...)
	 {

	 }
 
     return HandleCount;
 }
 
 ULONG PerfDataGetThreadCount(ULONG Index)
 {
     ULONG    ThreadCount = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			ThreadCount = g_pPerfData[Index].ThreadCount;
		else
			ThreadCount = 0;
	 }
	 catch(...)
	 {

	 }
     return ThreadCount;
 }
 
 ULONG PerfDataGetUSERObjectCount(ULONG Index)
 {
     ULONG    USERObjectCount = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			USERObjectCount = g_pPerfData[Index].USERObjectCount;
		else
			USERObjectCount = 0;
	 }
	 catch(...)
	 {
	 }
     return USERObjectCount;
 }
 
 ULONG PerfDataGetGDIObjectCount(ULONG Index)
 {
     ULONG    GDIObjectCount = 0;
	 try
	 {
		if (Index < g_ProcessCount)
			GDIObjectCount = g_pPerfData[Index].GDIObjectCount;
		else
			GDIObjectCount = 0;
	 }
	 catch(...)
	 {

	 }
     return GDIObjectCount;
 }
 
 BOOL PerfDataGetIOCounters(ULONG Index, PIO_COUNTERS pIoCounters)
 {
     BOOL    bSuccessful = FALSE;
	 try
	 {
		if (Index < g_ProcessCount)
		{
			memcpy(pIoCounters, &g_pPerfData[Index].IOCounters, sizeof(IO_COUNTERS));
			bSuccessful = TRUE;
		}
		else
			bSuccessful = FALSE;
	 }
	 catch(...)
	 {

	 }
     return bSuccessful;
 }
 
 ULONG PerfDataGetCommitChargeTotalK(void)
 {
     ULONG    Total		=	0;
     ULONG    PageSize	=	0;
 
	 try
	 {
		Total = g_SystemPerfInfo.MmTotalCommitedPages;
		PageSize = g_SystemBasicInfo.uPageSize;
		Total = Total * (PageSize / 1024);
	 }
	 catch(...)
	 {

	 }
     return Total;
 }
 
 ULONG PerfDataGetCommitChargeLimitK(void)
 {
     ULONG    Limit		=	0;
     ULONG    PageSize	=	0;
 
	 try
	 {
		Limit = g_SystemPerfInfo.MmTotalCommitLimit;
		PageSize = g_SystemBasicInfo.uPageSize;
		Limit = Limit * (PageSize / 1024);
	 }
	 catch(...)
	 {

	 }
     return Limit;
 }
 
 ULONG PerfDataGetCommitChargePeakK(void)
 {
     ULONG    Peak		=	0;
     ULONG    PageSize	=	0;
 
	 try
	 {
		Peak = g_SystemPerfInfo.MmPeakLimit;
		PageSize = g_SystemBasicInfo.uPageSize;
		Peak = Peak * (PageSize / 1024);
	 }
	 catch(...)
	 {

	 }
     return Peak;
 }
 
 ULONG PerfDataGetKernelMemoryTotalK(void)
 {
     ULONG    Total			=	0;
     ULONG    Paged			=	0;
     ULONG    NonPaged		=	0;
     ULONG    PageSize		=	0;
	 try
	 {
		Paged = g_SystemPerfInfo.PoolPagedBytes;
		NonPaged = g_SystemPerfInfo.PoolNonPagedBytes;
		PageSize = g_SystemBasicInfo.uPageSize;
	 
	 
		Paged = Paged * (PageSize / 1024);
		NonPaged = NonPaged * (PageSize / 1024);
	 
		Total = Paged + NonPaged;
	 }
	 catch(...)
	 {

	 }
 
     return Total;
 }
 
 ULONG PerfDataGetKernelMemoryPagedK(void)
 {
     ULONG    Paged			=	0;
     ULONG    PageSize		=	0;
	 try
	 {
		Paged = g_SystemPerfInfo.PoolPagedBytes;
		PageSize = g_SystemBasicInfo.uPageSize;
		Paged = Paged * (PageSize / 1024);
	 }
	 catch(...)
	 {

	 }
     return Paged;
 }
 
 ULONG PerfDataGetKernelMemoryNonPagedK(void)
 {
     ULONG    NonPaged		=	0;
     ULONG    PageSize		=	0;
 
	 try
	 {
		NonPaged = g_SystemPerfInfo.PoolNonPagedBytes;
		PageSize = g_SystemBasicInfo.uPageSize;
		NonPaged = NonPaged * (PageSize / 1024);
	 }
	 catch(...)
	 {

	 }
     return NonPaged;
 }
 
 ULONG PerfDataGetPhysicalMemoryTotalK(void)
 {
     ULONG    Total		=	0;
     ULONG    PageSize	=	0;
 
	 try
	 {
		Total = g_SystemBasicInfo.uMmNumberOfPhysicalPages;
		PageSize = g_SystemBasicInfo.uPageSize;
		Total = Total * (PageSize / 1024);
	 }
	 catch(...)
	 {

	 }
     return Total;
 }
 
 ULONG PerfDataGetPhysicalMemoryAvailableK(void)
 {
     ULONG    Available	=	0;
     ULONG    PageSize	=	0;
 
	 try
	 {
		Available = g_SystemPerfInfo.MmAvailablePages;
		PageSize = g_SystemBasicInfo.uPageSize;
		Available = Available * (PageSize / 1024);
	 }
	 catch(...)
	 {

	 }
     return Available;
 }
 
 ULONG PerfDataGetPhysicalMemorySystemCacheK(void)
 {
     ULONG    SystemCache	=	0;
     ULONG    PageSize		=	0;
 
	 try
	 {
		SystemCache = g_SystemCacheInfo.CurrentSize;
		PageSize = g_SystemBasicInfo.uPageSize;
		/* SystemCache = SystemCache * (PageSize / 1024); */
		SystemCache = SystemCache / 1024;
	 }
	 catch(...)
	 {

	 }
     return SystemCache;
 }
 
 ULONG PerfDataGetSystemHandleCount(void)
 {
     ULONG    HandleCount;
 
 
     HandleCount = g_SystemHandleInfo.Count;
 
 
     return HandleCount;
 }
 
 ULONG PerfDataGetTotalThreadCount(void)
 {
     ULONG    ThreadCount = 0;
     ULONG    i = 0;
 
	 try
	 {
		for (i=0; i<g_ProcessCount; i++)
		{
			ThreadCount += g_pPerfData[i].ThreadCount;
		}
	 }
	 catch(...)
	 {

	 }
     return ThreadCount;
 }
 
 
 ULONG	PerfGetProcessIdAndCPUMemoryUsage(LPTSTR lpImageName,
											ULONG* plProcId,
											ULONG* plCPUUsage,
											ULONG* plMemoryUsage)
 {
	ULONG	iProcCount						= 0;
	 try
	 {
		//First get the number of process
		ULONG	iNum							= PerfDataGetProcessCount();
		ULONG	i								= 0;
		char	szProcessName[MAX_PATH + 10]	= {0};
		char	szProcess[MAX_PATH + 10]		= {0};
		char*	pszFindChar						= NULL;

		pszFindChar = strchr(lpImageName,'.');
		if(NULL != pszFindChar)
		{
			*pszFindChar = NULL;
			lstrcpy(szProcess,lpImageName);
			*pszFindChar = '.';
		}

		for(i = 0; i < iNum; ++i)
		{
			memset(szProcessName,NULL,(MAX_PATH + 10));
			PerfDataGetImageName(i,szProcessName,MAX_PATH);
			if(0 == lstrcmpi(lpImageName,szProcessName))
			{
				plProcId[iProcCount]		=	PerfDataGetProcessId(i);
				plCPUUsage[iProcCount]		=	PerfDataGetCPUUsage(i);
				plMemoryUsage[iProcCount]	=	PerfDataGetWorkingSetSizeBytes(i);
				++iProcCount;
			}
			else
			{
				if(NULL != szProcess[0])
				{
					if(0 == lstrcmpi(szProcess,szProcessName))
					{
						plProcId[iProcCount]		=	PerfDataGetProcessId(i);
						plCPUUsage[iProcCount]		=	PerfDataGetCPUUsage(i);
						plMemoryUsage[iProcCount]	=	PerfDataGetWorkingSetSizeBytes(i);
						++iProcCount;
					}
					else
					{
						pszFindChar = strchr(szProcessName,'.');
						if(NULL != pszFindChar)
						{
							*pszFindChar = NULL;
							if(0 == lstrcmpi(szProcess,szProcessName))
							{
								plProcId[iProcCount]		=	PerfDataGetProcessId(i);
								plCPUUsage[iProcCount]		=	PerfDataGetCPUUsage(i);
								plMemoryUsage[iProcCount]	=	PerfDataGetWorkingSetSizeBytes(i);
								++iProcCount;
							}
						}
					}
				}
			}
		}
	 }
	 catch(...)
	 {

	 }
	return 	iProcCount;
 }


ULONG	DisplayPerfGetProcessIdAndCPUMemoryUsage()
{
	 //First get the number of process
	ULONG iNum = PerfDataGetProcessCount();
	ULONG i    = 0;
	ULONG iProcCount = 0;
	char	szProcessName[MAX_PATH + 10] = {0};
	char	szProcess[MAX_PATH + 10]	 = {0};

	printf("\n%-25s%-15s%-15s%15s","Image Name","PID","CPU","Mem Usage");
	printf("\n%-25s%-15s%-15s%15s","----------","---","---","---------");

	for(i = 0; i < iNum; ++i)
	{
		memset(szProcessName,NULL,(MAX_PATH + 10));
		PerfDataGetImageName(i,szProcessName,MAX_PATH);
		printf("\n%-25s%-15ld%-15ld%15ld K",
				szProcessName,
				PerfDataGetProcessId(i),
				PerfDataGetCPUUsage(i),
				(PerfDataGetWorkingSetSizeBytes(i)/1024));
	}
	printf("\n\nTotal Processes  %ld\n\n",iNum);
	return 	iProcCount;
 }
