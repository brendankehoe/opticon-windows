// OPN2001_DEMO.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// COPN2001_DEMOApp:
// See OPN2001_DEMO.cpp for the implementation of this class
//

class COPN2001_DEMOApp : public CWinApp
{
public:
	COPN2001_DEMOApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern COPN2001_DEMOApp theApp;