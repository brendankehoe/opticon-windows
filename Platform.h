//
// Platform.cpp
//
// Jason Schurr
//
// Intended for use to determine the version of Windows and platform specific variables such as Application Data folders

CString GetAppDataPath( void );
BOOL DirectoryExists(LPCTSTR szPath);
BOOL CreateAppDataDirectory(LPCTSTR szPath);
extern CString platformError;