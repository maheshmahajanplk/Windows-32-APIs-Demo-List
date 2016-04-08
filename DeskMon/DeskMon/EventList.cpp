#include "stdafx.h"


void	AddToList(stServiceEvent*&	pRoot,char*	pszServiceName,int iEventId)
{
	stServiceEvent*		pTemp		=		NULL;
	__try
	{
		__try
		{
			if(NULL == pRoot)
			{
				pRoot	=	new stServiceEvent;
				if(NULL == pRoot)
				{
					return;
				}
				memset(pRoot,NULL,sizeof(stServiceEvent));
				lstrcpy(pRoot->szServiceName,pszServiceName);
				pRoot->iEventId = iEventId;
			}
			else
			{
				pTemp = new stServiceEvent;
				if(NULL == pTemp)
				{
					return;
				}
				memset(pTemp,NULL,sizeof(stServiceEvent));
				lstrcpy(pTemp->szServiceName,pszServiceName);
				pTemp->iEventId = iEventId;

				pTemp->pNext = pRoot;
				pRoot = pTemp;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{

		}
	}
	__finally
	{

	}
}

void	SearchList(stServiceEvent*&	pRoot,char*	pszServiceName,int& iEventId)
{
	stServiceEvent*		pTemp		=		NULL;
	__try
	{
		__try
		{
			iEventId = 0;

			pTemp = pRoot;
			while(NULL != pTemp)
			{
				if(0 == lstrcmpi(pszServiceName,pTemp->szServiceName))
				{
					iEventId = pTemp->iEventId;
					break;
				}
				pTemp = pTemp->pNext;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{

		}
	}
	__finally
	{

	}
}