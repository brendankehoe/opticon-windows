// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DEF_PROFILE_THRESHOLD	512 // temporary string length

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile()
	:m_strFilePathName( _T("") )
{

}

CIniFile::CIniFile( const CString strFilePathName )
{
	SetPathName( strFilePathName );
}

CIniFile::~CIniFile()
{

}

void CIniFile::SetPathName( const CString strFilePathName )
{
	m_strFilePathName = strFilePathName;
}

CString CIniFile::GetPathName( void ) const
{
	return m_strFilePathName;
}


CString CIniFile::ReadString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault ) const
{
	LPTSTR psz = __GetStringDynamic(lpSection, lpKey, lpDefault);
	CString str(psz);
	delete [] psz;
	return str;
}

BOOL CIniFile::WriteString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue) const
{
	if (lpSection == NULL || lpKey == NULL)
		return FALSE;

	return ::WritePrivateProfileString(lpSection, lpKey, lpValue == NULL ? _T("") : lpValue, m_strFilePathName);
}


int CIniFile::ReadInt(LPCTSTR lpSection, LPCTSTR lpKey, int nDefault ) const
{
	CString str;
	str = ReadString( lpSection, lpKey );

	return (str.IsEmpty())? nDefault : (int)_tcstoul( str, NULL, 10 );
}

BOOL CIniFile::WriteInt(LPCTSTR lpSection, LPCTSTR lpKey, int nValue ) const
{
	CString str;
	str.Format(_T("%d"), nValue );
	return WriteString( lpSection, lpKey, str );
}
	
UINT CIniFile::ReadUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nDefault ) const
{
	CString str;
	str = ReadString( lpSection, lpKey );

	return (str.IsEmpty())? nDefault : (UINT)_tcstoul( str, NULL, 10 );
}

BOOL CIniFile::WriteUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nValue ) const
{
	CString str;
	str.Format(_T("%u"), nValue );
	return WriteString( lpSection, lpKey, str );
}
	
BOOL CIniFile::ReadBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bDefault) const
{
	CString str;
	str = ReadString( lpSection, lpKey );
	return StringToBool( str, bDefault);
}

BOOL CIniFile::WriteBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bValue) const
{
	return WriteInt(lpSection, lpKey, bValue ? 1 : 0 );
}



/////////////////////////////////////////////////////////////////////////////////
// Helper Functions
/////////////////////////////////////////////////////////////////////////////////


// Get a profile string value, return a heap pointer so we do not have to worry
// about the buffer size, however, this function requires the caller to manually
// free the memory.
// This function is the back-bone of all "Getxxx" functions of this class.
LPTSTR CIniFile::__GetStringDynamic(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault) const
{
	WCHAR* psz = NULL;
	if( lpSection == NULL || lpKey == NULL)
	{
		// Invalid section or key name, just return the default string
		if (lpDefault == NULL)
		{
			// Empty string
			psz = new TCHAR[1];
			*psz = _T('\0');
		}
		else
		{
			psz = new WCHAR[ wcslen(lpDefault) + 1];
			wcscpy_s(psz, wcslen(lpDefault) + 1, lpDefault);
		}
		return psz;
	}
	
	// Keep enlarging the buffer size until being certain on that the string we
	// retrieved was original(not truncated).
	DWORD dwLen = DEF_PROFILE_THRESHOLD;
	psz = new TCHAR[dwLen + 1];
	DWORD dwCopied = ::GetPrivateProfileString(lpSection, lpKey, lpDefault == NULL ? _T("") : lpDefault, psz, dwLen, m_strFilePathName);
	while( dwCopied + 1 >= dwLen )
	{		
		dwLen += DEF_PROFILE_THRESHOLD;
		delete [] psz;
		psz = new TCHAR[dwLen + 1];
		dwCopied = ::GetPrivateProfileString(lpSection, lpKey, lpDefault == NULL ? _T("") : lpDefault, psz, dwLen, m_strFilePathName);
	}
	
	return psz; // !!! Requires the caller to free this memory !!!
}

BOOL CIniFile::StringToBool( const CString strVal, BOOL bDefault)
{
	// Default: empty string
	// TRUE: "true", "yes", non-zero decimal numner
	// FALSE: all other cases
	if( strVal.IsEmpty() )
		return bDefault;

	return ( strVal.CompareNoCase( _T("TRUE")) == 0
		|| strVal.CompareNoCase( _T("YES")) == 0
		|| _tcstol( strVal, NULL, 10 ) != 0 );
}
