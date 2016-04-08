#include "stdafx.h"
#include "Socket.h"
//#include <fstream.h>

#define WINSOCK_INITIALIZED						1
#define MAX_BUFFER_LEN							255
#define MAX_LONG_SIZE							512



CSAMSocket::CSAMSocket(int nInitialized, sockaddr_in * valSock, SOCKET sockVar) : m_nSock(sockVar), m_nInitialized(nInitialized)
{
	int iRetVal = 0;
	
	if (valSock != NULL)
	{
		m_SamSocket = *valSock;
	}
	/*
	if (m_nInitialized == WINSOCK_INITIALIZED) 
	{
		return;
	}
	
	iRetVal = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iRetVal != 0) 
	{
		m_nInitialized = 0;
	}
	*/
	m_nInitialized = WINSOCK_INITIALIZED;
	m_nLastError = 0;
}

CSAMSocket::~CSAMSocket()
{

}

int CSAMSocket::Create(int af, int type, int proto)
{
	
	m_nSock = socket(af, type, proto);

	if( m_nSock != INVALID_SOCKET)
	{
		return m_nSock;
	}
	else 
	{
		m_nLastError = WSAGetLastError();
		return 0 ;
	}
	
	
}

int CSAMSocket::Bind(int nPort, char *szSocketAddress) 
{	
	
	unsigned long lAddress = htonl(INADDR_ANY);
	if (szSocketAddress != NULL) {
		lAddress = inet_addr(szSocketAddress);
		if (lAddress == INADDR_NONE)
		{	
			//return 0 ;
		}
	}
	
	m_SamSocket.sin_family = AF_INET;
	m_SamSocket.sin_port = htons(nPort);
	m_SamSocket.sin_addr.S_un.S_addr = lAddress;
	int iRetVal = bind(m_nSock, (SOCKADDR *)&m_SamSocket, sizeof(m_SamSocket));
	if (iRetVal == SOCKET_ERROR) 
	{
		return 0 ;
	}
	return 1;		
	
}

int CSAMSocket::Listen(int nBackLog) 
{
	
	if(listen(m_nSock, nBackLog) == SOCKET_ERROR)	
	{
		return 0 ; // on failure 
	}
	return 1 ; // on success returns 0
	
}

CBaseCommunication *CSAMSocket::Accept()
{			
	CBaseCommunication *pSAMSocket=NULL;
	int clientSocket;
	sockaddr_in clientSockAddr = { 0 };
	int lenSockAddr = sizeof(clientSockAddr);
	
	clientSocket = accept(m_nSock, (SOCKADDR *)&clientSockAddr, &lenSockAddr);
	
	pSAMSocket = new CSAMSocket(WINSOCK_INITIALIZED, &clientSockAddr, clientSocket);
	
	return pSAMSocket;			
}

int CSAMSocket::Send(char* lpBuf, long nBufLen, int nFlags)
{		
	long nCount = 0;
	int nBytesSent = 0;

	while (nCount < nBufLen) {
		nBytesSent = send(m_nSock, (lpBuf + nCount), ((nCount + MAX_LONG_SIZE)>nBufLen)?(nBufLen -  nCount):MAX_LONG_SIZE, nFlags);
		if (nBytesSent == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		nCount += nBytesSent;
	}
	return nCount;
}

int CSAMSocket::Receive(char * lpBuf, long nBufLen, int nFlags)
{	
	long nCount = 0;
	int nBytesSent = 0;

	//while (nCount < nBufLen) {
		nBytesSent = recv(m_nSock,lpBuf,nBufLen,nFlags);
			//m_nSock, (lpBuf + nCount), ((nCount + MAX_LONG_SIZE)>nBufLen)?(nBufLen -  nCount):MAX_LONG_SIZE, nFlags);
		if (nBytesSent == SOCKET_ERROR || nBytesSent == 0) {
			return SOCKET_ERROR;
		}
	//	nCount += nBytesSent;
	//}
		
	return nBytesSent;
}

void CSAMSocket::Close(void )
{
	shutdown(m_nSock,SD_BOTH);
	closesocket(m_nSock);
	m_nSock = 0;
	memset(&m_SamSocket,NULL,sizeof(m_SamSocket));
}

int CSAMSocket::Connect(const char *szAddress, int nPort)
{
	
	unsigned long lAddress = htonl(INADDR_ANY);
	if (szAddress == NULL) {
		return 0;
	}

	lAddress = inet_addr(szAddress);
	if (lAddress == INADDR_NONE) {
		return 0;
	}
	
	m_SamSocket.sin_family = AF_INET;
	m_SamSocket.sin_port = htons(nPort);
	m_SamSocket.sin_addr.S_un.S_addr = lAddress;
	int iRetVal = connect(m_nSock, (SOCKADDR *)&m_SamSocket, sizeof(m_SamSocket));

	if(0 != iRetVal)
	{
		m_nLastError = WSAGetLastError();
	}
	
	return (iRetVal==-1)?0:1 ;
}
int CSAMSocket:: SetSocketTimeout(int iTimeOut)
{
	int	timeout = 0;
	__try
	{
		__try
		{
			if(iTimeOut <= 0)
			{
				return 0;
			}

			timeout = iTimeOut;
			if (setsockopt(m_nSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
								sizeof(timeout)) == SOCKET_ERROR)
			{
				return 0;
			}

			timeout = iTimeOut;
			if (setsockopt(m_nSock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
								sizeof(timeout)) == SOCKET_ERROR)
			{
				return 0;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{


		}
	}
	__finally
	{


	}
	return 1;
}

void	CSAMSocket::GracefullClose()
{
	char*	buffer				=	NULL;
	char	stackBuffer[501]	=	{0};
	int		iSize				=	(1024 * 4);
	int		iBytesRecv			=	1;

	int		iCtr				=	0;

	__try
	{
		__try
		{
			buffer = new char[1024 * 4];
			if(NULL == buffer)
			{
				buffer = stackBuffer;
				iSize = 501;
			}

			//ok now its time to do a send shutdown
			shutdown(m_nSock,SD_SEND);

			while((iBytesRecv != SOCKET_ERROR) &&
					(iBytesRecv != 0))
			{
				iBytesRecv = 0;
				iBytesRecv = recv(m_nSock,buffer,iSize,0);
				++iCtr;

				if(iCtr > 10)
				{
					break;
				}
			}

			closesocket(m_nSock);
			m_nSock = 0;
			memset(&m_SamSocket,NULL,sizeof(m_SamSocket));
		
			if((1024 * 4) == iSize)
			{
				delete [] buffer;
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
