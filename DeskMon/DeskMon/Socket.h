#ifndef __SOCKET_H_
#define __SOCKET_H_
//#include <afx.h>
#include "BaseCommunication.h"
class CBaseCommunication ;
class CSAMSocket: public CBaseCommunication
{
public:
	int m_nLastError;

private:
//	CString m_Buffer;
	int m_nSock;
	sockaddr_in m_SamSocket;
	WSADATA wsaData;   
	int m_nInitialized;
public:
	int Connect(const char *szAddress, int nPort);
	int Create(int af = AF_INET, int type = SOCK_STREAM, int proto = 0);
	int Bind(int nPort, char *szSocketAddress = NULL);
	int Listen(int nBackLog = 0);
	int Send(char * lpBuf, long nBufLen, int nFlags = 0);
	int Receive(char * lpBuf, long nBufLen, int nFlags = 0);
	void Close();
	void GracefullClose();
	int SetSocketTimeout(int iTimeOut);
	//char *GetBuffer() { return m_Buffer.GetBuffer(0); };
	CBaseCommunication *Accept();
	CSAMSocket(int nInitialized = 0, sockaddr_in *valSock = 0, SOCKET sockVar = INVALID_SOCKET);
	virtual ~CSAMSocket();
};

#endif /* __SOCKET_H_ */