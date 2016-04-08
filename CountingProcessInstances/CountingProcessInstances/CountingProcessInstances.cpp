// CountingProcessInstances.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{

		/*---------FUNCTION WinMain()----------- */
	long GlobalApplicationCount;
	int ReturnValue;
	HANDLE ApplicationCountSemaphore;
	const TCHAR szSemaphoreName[] = TEXT("XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX");
	const int MaxApplicationCount = 0x0FFFFFFF;

	// Create a semaphore to count program instances
	ApplicationCountSemaphore = CreateSemaphore(NULL, MaxApplicationCount,MaxApplicationCount, szSemaphoreName);
	if(ApplicationCountSemaphore == NULL)
	{
		// CreateSemaphore error, use GetLastError()
	}

	// Wait on the semaphore (possible existing) to decrement its count
	WaitForSingleObject(ApplicationCountSemaphore, 0L);
	// Wait on the semaphore again
	WaitForSingleObject(ApplicationCountSemaphore, 0L);
	// Release the semaphore once to get the previous count
	ReleaseSemaphore(ApplicationCountSemaphore, 1, &GlobalApplicationCount);
	// Calculate the number of processes (including ourself)
	GlobalApplicationCount = MaxApplicationCount - GlobalApplicationCount - 1;

	// Run the main program function and catch any exceptions
	try
	{
		ReturnValue = WinMainProtected(hInstance,hInstUnused, lpCmdLine, nCmdShow);
	}
	catch(...)
	{
		MessageBox(NULL, TEXT("An unhandled exception occurred."), 
		NULL, MB_ICONEXCLAMATION | MB_OK);
		ReturnValue = -1;
	}

		// Release the semaphore
		ReleaseSemaphore(ApplicationCountSemaphore, 1, &GlobalApplicationCount);
		// Close the semaphore if we are the last process
		if(GlobalApplicationCount == MaxApplicationCount - 1)
		{
			CloseHandle(ApplicationCountSemaphore);
		}

		return ReturnValue;
	/*---------END OF WinMain()-----------*/
	return 0;
}

