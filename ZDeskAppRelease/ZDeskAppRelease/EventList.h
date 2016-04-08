
#include<WinDef.h>
struct	stServiceEvent
{
	char				szServiceName[MAX_PATH];
	int					iEventId;
	stServiceEvent*		pNext;
};

void	AddToList(stServiceEvent*&	pRoot,char*	pszServiceName,int iEventId);
void	SearchList(stServiceEvent*&	pRoot,char*	pszServiceName,int& iEventId);