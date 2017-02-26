#include <stdafx.h>
#include <aclapi.h>

CString platformError = CString("Error");

CString GetAppDataPath( void )
{
	TCHAR szPath[MAX_PATH];
	CString path = CString("");

	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath))) 
	{
		path = CString(szPath);
		return path;
		//return platformError;
	}

	return platformError;
}

BOOL DirectoryExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL CreateAppDataDirectory(LPCTSTR lpPath)
{
  if(!CreateDirectory(lpPath,NULL))
  {
	  return FALSE;
  }

  HANDLE hDir = CreateFile(lpPath,READ_CONTROL|WRITE_DAC,0,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
  if(hDir == INVALID_HANDLE_VALUE)
  {
	  return FALSE;
  }

  ACL* pOldDACL;
  SECURITY_DESCRIPTOR* pSD = NULL;
  GetSecurityInfo(hDir, SE_FILE_OBJECT , DACL_SECURITY_INFORMATION,NULL, NULL, &pOldDACL, NULL, (void**)&pSD);

  PSID pSid = NULL;
  SID_IDENTIFIER_AUTHORITY authNt = SECURITY_NT_AUTHORITY;
  AllocateAndInitializeSid(&authNt,2,SECURITY_BUILTIN_DOMAIN_RID,DOMAIN_ALIAS_RID_USERS,0,0,0,0,0,0,&pSid);

  EXPLICIT_ACCESS ea={0};
  ea.grfAccessMode = GRANT_ACCESS;
  ea.grfAccessPermissions = GENERIC_ALL;
  ea.grfInheritance = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
  ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
  ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
  ea.Trustee.ptstrName = (LPTSTR)pSid;

  ACL* pNewDACL = 0;
  /*DWORD err = */SetEntriesInAcl(1,&ea,pOldDACL,&pNewDACL);

  if(pNewDACL)
  {
	  SetSecurityInfo(hDir,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL, NULL, pNewDACL, NULL);
	  LocalFree(pNewDACL);
  }

  if(pSid)
	FreeSid(pSid);
  
  if(pSD)
	LocalFree(pSD);

  // LocalFree(pOldDACL); caused corrupted stack
  
  CloseHandle(hDir);
  return TRUE;
}