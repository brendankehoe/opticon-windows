// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////
// Martin Jansen
// Home brew project


#if !defined(AFX_INIFILE_H__42B8236E_1C85_4055_9AEA_EC1AFBA06ADF__INCLUDED_)
#define AFX_INIFILE_H__42B8236E_1C85_4055_9AEA_EC1AFBA06ADF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIniFile  
{
public:
	CIniFile();
	CIniFile( const CString strFilePathName );
	virtual ~CIniFile();

	void SetPathName( const CString strFilePathName );
	CString GetPathName( void ) const;
	
	//
	// Reading and writing string values
	//
	CString ReadString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault = NULL) const;
	BOOL WriteString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue) const;


	//
	// Reading and writing some int values
	//
	int ReadInt(LPCTSTR lpSection, LPCTSTR lpKey, int nDefault ) const;
	BOOL WriteInt(LPCTSTR lpSection, LPCTSTR lpKey, int nValue) const;
	
	UINT ReadUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nDefault) const;
	BOOL WriteUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nValue) const;
	
	BOOL ReadBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bDefault) const;
	BOOL WriteBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bValue) const;

private:
	CString m_strFilePathName;

	LPTSTR __GetStringDynamic(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault) const;
	static BOOL StringToBool( const CString strVal, BOOL bDefault = FALSE );
};

#endif // !defined(AFX_INIFILE_H__42B8236E_1C85_4055_9AEA_EC1AFBA06ADF__INCLUDED_)
