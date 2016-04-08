// BaseCommunication.h: interface for the CBaseCommunication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASECOMMUNICATION_H__33FAE0CB_B3BC_41F3_8525_3DB15C96BECD__INCLUDED_)
#define AFX_BASECOMMUNICATION_H__33FAE0CB_B3BC_41F3_8525_3DB15C96BECD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
enum COMMUNICATION_MEDIUM { TCP_IP_SOCKET=0,PIPE=1} ;

class CBaseCommunication  
{
public:
	CBaseCommunication();
	virtual ~CBaseCommunication();
	virtual int Connect(const char *szAddress, int nPort) = 0;
	virtual int Create(int af = AF_INET, int type = SOCK_STREAM, int proto = 0) {return 1;};
	virtual int Bind(int nPort, char *szSocketAddress = NULL) {return 1;};
	virtual int Listen(int nBackLog = 5) {return 1;} ;
	virtual int Send(char * lpBuf, long nBufLen, int nFlags = 0) = 0;
	virtual int Receive(char * lpBuf, long nBufLen, int nFlags = 0) = 0;
	virtual void Close() = 0 ;
	virtual CBaseCommunication *Accept() = 0;
//	virtual char *GetBuffer() {return (char *)1;} //{ return (char *)(LPCTSTR)m_Buffer; };
	COMMUNICATION_MEDIUM GetCommunicationMedium() { return m_CommMedium ;} ;
	void SetCommunicationMedium(COMMUNICATION_MEDIUM CommMedium) {m_CommMedium = CommMedium;} ;
	
protected:	
	COMMUNICATION_MEDIUM m_CommMedium ;
private:
};

#endif // !defined(AFX_BASECOMMUNICATION_H__33FAE0CB_B3BC_41F3_8525_3DB15C96BECD__INCLUDED_)
