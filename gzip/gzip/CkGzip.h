// CkGzip.h: interface for the CkGzip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CK_GZIP_H)
#define _CK_GZIP_H


#include "CkMultiByteBase.h"
#include <Windows.h>
#include "CkString.h"
class CkByteData;
class CkDateTime;

#if !defined(WIN32) && !defined(WINCE)
#include "SystemTime.h"
#endif

class CkGzipProgress;

#ifndef __sun__
#pragma pack (push, 8)
#endif


// CLASS: CkGzip
class CkGzip : public CkMultiByteBase
{
public:
private:
	CkGzipProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkGzip(const CkGzip &) { }
	CkGzip &operator=(const CkGzip &) { return *this; }
	CkGzip(void *impl);

public:
	CkGzipProgress *get_EventCallbackObject(void) const { return m_callback; }
	void put_EventCallbackObject(CkGzipProgress *progress) { m_callback = progress; }

	CkGzip();
	virtual ~CkGzip();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// SETDT_BEGIN
	bool SetDt(CkDateTime &dt);
	// SETDT_END

	// GETDT_BEGIN
	CkDateTime *GetDt(void);
	// GETDT_END


	bool UnlockComponent(const char *unlockCode);
	bool IsUnlocked(void) const;


	// File-to-file
	bool CompressFile(const char *inFilename, const char *outFilename);
	bool UncompressFile(const char *inFilename, const char *outFilename);

	// File-to-memory
	bool CompressFileToMem(const char *inFilename, CkByteData &outData);
	bool UncompressFileToMem(const char *inFilename, CkByteData &outData);

	// Memory-to-file
	bool CompressMemToFile(const CkByteData &db, const char *outFilename);
	bool UncompressMemToFile(const CkByteData &db, const char *outFilename);

	// Memory-to-memory
	bool CompressMemory(const CkByteData &dbIn, CkByteData &outData);
	bool UncompressMemory(const CkByteData &dbIn, CkByteData &outData);

	// String deflate/inflate
	bool DeflateStringENC(const char *str, const char *charset, const char *encoding, CkString &outStr);
	bool InflateStringENC(const char *str, const char *charset, const char *encoding, CkString &outStr);
	const char *deflateStringENC(const char *str, const char *charset, const char *encoding);
	const char *inflateStringENC(const char *str, const char *charset, const char *encoding);

	void get_Filename(CkString &str);
	void put_Filename(const char *str);

	void get_Comment(CkString &str);
	void put_Comment(const char *str);

	void get_ExtraData(CkByteData &data);
	void put_ExtraData(const CkByteData &data);

	// SYSTEMTIME is defined in <windows.h>
	// For more information, Google "SYSTEMTIME"
	void get_LastMod(SYSTEMTIME &outSysTime);
	void put_LastMod(SYSTEMTIME &sysTime);

	bool UnTarGz(const char *gzFilename, const char *destDir, bool bNoAbsolute);

	const char *comment(void);
	const char *filename(void);
	// VERSION_BEGIN
	void get_Version(CkString &str);
	const char *version(void);
	// VERSION_END
	// READFILE_BEGIN
	bool ReadFile(const char *filename, CkByteData &outBytes);
	// READFILE_END
	// WRITEFILE_BEGIN
	bool WriteFile(const char *filename, CkByteData &binaryData);
	// WRITEFILE_END
	// COMPRESSFILE2_BEGIN
	bool CompressFile2(const char *inFilename, const char *embeddedFilename, const char *outFilename);
	// COMPRESSFILE2_END
	// COMPRESSSTRING_BEGIN
	bool CompressString(const char *inStr, const char *outCharset, CkByteData &outBytes);
	// COMPRESSSTRING_END
	// COMPRESSSTRINGTOFILE_BEGIN
	bool CompressStringToFile(const char *inStr, const char *outCharset, const char *outFilename);
	// COMPRESSSTRINGTOFILE_END
	// UNCOMPRESSFILETOSTRING_BEGIN
	bool UncompressFileToString(const char *inFilename, const char *inCharset, CkString &outStr);
	const char *uncompressFileToString(const char *inFilename, const char *inCharset);
	// UNCOMPRESSFILETOSTRING_END
	// UNCOMPRESSSTRING_BEGIN
	bool UncompressString(CkByteData &inData, const char *inCharset, CkString &outStr);
	const char *uncompressString(CkByteData &inData, const char *inCharset);
	// UNCOMPRESSSTRING_END
	// EXAMINEFILE_BEGIN
	bool ExamineFile(const char *inGzFilename);
	// EXAMINEFILE_END
	// EXAMINEMEMORY_BEGIN
	bool ExamineMemory(CkByteData &inGzData);
	// EXAMINEMEMORY_END
	// DECODE_BEGIN
	bool Decode(const char *str, const char *encoding, CkByteData &outBytes);
	// DECODE_END
	// USECURRENTDATE_BEGIN
	bool get_UseCurrentDate(void);
	void put_UseCurrentDate(bool newVal);
	// USECURRENTDATE_END
	// XFDLTOXML_BEGIN
	bool XfdlToXml(const char *xfdl, CkString &outStr);
	const char *xfdlToXml(const char *xfdl);
	// XFDLTOXML_END
	// ENCODE_BEGIN
	bool Encode(CkByteData &byteData, const char *encoding, CkString &outStr);
	const char *encode(CkByteData &byteData, const char *encoding);
	// ENCODE_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END

	// GZIP_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif // !defined(_CK_GZIP_H)