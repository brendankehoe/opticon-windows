#pragma once
#include "afxcmn.h"
#include ".\gridctrl\GridCtrl.h"
#include "ProgressWnd.h"
#include "OPN2001_DEMODlg.h"
#include <list>


// CDlgParameters dialog

class CDlgParameters : public CDialog
{
	DECLARE_DYNAMIC(CDlgParameters)

public:
	CDlgParameters(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgParameters();

// Dialog Data
	enum { IDD = IDD_DLG_PARAMETERS };

protected:
	CStringArray	m_paramStringOptions[ 15 ];
	CProgressWnd	*m_pProgWnd;

	void ShowErrorMessage( long lErrorCode );
	virtual void OnOK();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void FillListControl( void );
    afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* pResult);


	DECLARE_MESSAGE_MAP()
public:
	CGridCtrl m_gridParam;
	long ComPort;
	std::list<SOpn200xStatus*> ComPortList;
	afx_msg void OnBnClickedBtnDefault();
	afx_msg void OnBnClickedBtnSettings();
	afx_msg void OnBnClickedBtnSettings(long nComPort);
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnApplyToAll();
};
