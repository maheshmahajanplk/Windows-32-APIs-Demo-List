// WriteToBuffer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WriteToBuffer.h"
#include <stdio.h>
#include <string.h>
#include<malloc.h>
#include <exception>


#define  g_dwExtraBuffer	 256


HRESULT WriteToBuffer(LPTSTR	&lpszBuffer,	//[in,out]Destination pointer where the buffer is located
						LPCTSTR lpszString,		//[in]String to be copied to the buffer
						BOOL bRefresh)			//[in]Initialize the buffer if it is TRUE, default is FALSE
{
	DWORD dwSize = 0;
	try
	{
		if(NULL == lpszString)
		{
			return E_POINTER;
		}

		//Get the actual size of the string
		dwSize = _tcslen(lpszString);

		if(NULL == lpszBuffer)
		{
			//allocate the required initial memory
			DWORD dwMemoryReq = g_dwExtraBuffer + dwSize;

			lpszBuffer = new TCHAR [dwMemoryReq + 1];
			if(NULL == lpszBuffer)
			{
				//allocation failed
				return ERROR_NOT_ENOUGH_MEMORY;
			}

			//allocation successful, set the memory
			//_tcsnset(lpszBuffer,_T('\0'),dwMemoryReq);

			memset(lpszBuffer,_T('\0'),dwMemoryReq);
			//copy the source string to the destination
			_tcscpy(lpszBuffer,lpszString);
		}
		else
		{
			//Check for the validity of the lpszBuffer pointer
			BOOL bRet1 = IsBadWritePtr(lpszBuffer,1);
			BOOL bRet2 = IsBadReadPtr(lpszBuffer,1);
			if(0 != bRet1 ||
				0 != bRet2)
			{
				//bad memory pointer, allocate a new memory
				DWORD dwMemoryReq = g_dwExtraBuffer + dwSize;
				lpszBuffer = new TCHAR [dwMemoryReq + 1];
				if(NULL == lpszBuffer)
				{
					return ERROR_NOT_ENOUGH_MEMORY;
				}
				//allocation successful, set the memory
				//_tcsnset(lpszBuffer,_T('\0'),dwMemoryReq);
				memset(lpszBuffer,_T('\0'),dwMemoryReq);
			}

			//see whether the current memory is sufficient to add the string to the buffer
			DWORD dwActualSize = _msize(lpszBuffer);
			--dwActualSize;
			DWORD dwSizeUsed = _tcslen(lpszBuffer);

			//Initialize the buffer if set to TRUE
			if(TRUE == bRefresh)
			{
				//_tcsnset(lpszBuffer,_T('\0'),dwActualSize);
				memset(lpszBuffer,_T('\0'),dwActualSize);
				dwSizeUsed = 0;
			}

			//calculate the required size for the store string & the string to be stored
			DWORD dwReqSize = dwSizeUsed + dwSize + 1;
			if(dwReqSize > dwActualSize)
			{
				//Memory is insufficient allocate a new block
				DWORD dwMemoryReq = g_dwExtraBuffer + dwReqSize;

				LPTSTR lpszTemp = new TCHAR[dwMemoryReq + 1];
				if(NULL == lpszTemp)
				{
					//allocation failed
					return ERROR_NOT_ENOUGH_MEMORY;
				}
				//allocation found successful, initialize the mem block
				//_tcsnset(lpszTemp,_T('\0'),dwMemoryReq);
				memset(lpszTemp,_T('\0'),dwMemoryReq);

				//copy the old string back to the newly allocated block
				_tcscpy(lpszTemp,lpszBuffer);

				//delete the old allocated block
				try
				{
					delete [] lpszBuffer;
				}
				catch(...)
				{
					//Something has gone wrong while deleting, it might be Corrupt Memory
					lpszBuffer = NULL;
					//Store the newly allocated memory
					lpszBuffer = lpszTemp;

					//copy the final string
					_tcscpy(lpszBuffer,lpszString);
					return S_OK;
				}
				lpszBuffer = NULL;
				lpszBuffer = lpszTemp;

				//copy the final string
				_tcscat(lpszBuffer,lpszString);
			}
			else
			{
				//Memory is sufficient copy the string
				_tcscat(lpszBuffer,lpszString);
			}
		}
	}
	catch(...)
	{
		//some thing has gone wrong ???????
		return E_FAIL;
	}
	return S_OK;
}



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	try{
				char * pszDataBuffer;
				char pszNormVal[20];
				strcpy(pszNormVal,"234567890");

				WriteToBuffer(pszDataBuffer,"<diskdetail>\r\n",FALSE);	
				WriteToBuffer(pszDataBuffer,"<issmartenabled>",FALSE);
				WriteToBuffer(pszDataBuffer,"0",FALSE);
				WriteToBuffer(pszDataBuffer,"</issmartenabled>\r\n",FALSE);

				WriteToBuffer(pszDataBuffer,"<diskid>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</diskid>\r\n",FALSE);

				WriteToBuffer(pszDataBuffer,"<id>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</id>\r\n",FALSE);
				WriteToBuffer(pszDataBuffer,pszNormVal,FALSE);

				delete[] pszDataBuffer;
	}
	catch(std::exception e )
	{
		try
		{

			OutputDebugString("ERR\tException Occur in WinMain.");
			
		}
		catch(...)
		{

		}
	}
	return 0;
}



