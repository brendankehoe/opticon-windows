// OPN2001_DEMODlg.h : header file
//

#pragma once

#include <list>
#include "afxwin.h"


typedef struct 
{
	LPCWSTR szSymbolName;
	long	lSymbol;
}SBCD_DEFS;

#define FIRMWARE_UPDATE				2
#define FIRMWARE_UPDATE_FAILED		3

typedef struct Opn200xStatus
{
	int		index;					// Used as quick reference of the position in m_DeviceArray
	long	nComPort;				// Serial port number 1L means COM1
	char	iConnected;				// TRUE if connected, FALSE if not, 2=Busy with firmware update
	long    nReadBarcodes;			// number of extracted barcodes
	CString sDeviceId;			    // Device ID
	CString sSwVersion;				// Device software version

	 // Assignment operator.
	bool operator == (const Opn200xStatus& st)
	{
      return (nComPort == st.nComPort);
	}
}SOpn200xStatus;


// COPN2001_DEMODlg dialog
class COPN2001_DEMODlg : public CDialog
{
// Construction
public:
	COPN2001_DEMODlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OPN2001_DEMO_DIALOG };

	static void *pDlgObject;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:

	#define WM_TRAY_MESSAGE (WM_USER + 1)

	std::list<SOpn200xStatus*> m_DeviceArray;

	std::list<int> m_nPortArray;
	std::list<int>::iterator m_itPorts;
	
	int m_nUsedPort;
	bool m_bMinimized;
	RECT WindowRect;
	NOTIFYICONDATA m_TrayData;

// Implementation
protected:
	HICON m_hIcon;
	CString m_strConfigFile;	// holds the previously used serial port
	BOOL	m_bSaveToFile;
	CString m_strSaveFile;
	CString m_strFirmwareFile;
	BOOL	m_bAuto;
	BOOL	m_bAutoNotClicked;
	BOOL	m_bShowAll;
	BOOL	m_bDetails;
	long	m_lComOpen;
	CToolTipCtrl m_ToolTip;

	virtual void OnOK();
	virtual void OnCancel();
	BOOL StartAutomaticDownload( void );
	BOOL StopAutomaticDownload( void );
	void UnpackCurrentTime( char *packedTimeandDate, CString &unpackedTime);
	void UnpackCurrentDate( char *packedTimeandDate, CString &unpackedDate);
	void GenerateHeader( CString &strTemp );
	//void ConvertBarcodeToString( char *szBarcode, CString &strTemp, long lAsciiMode, long lRTC);
	void ConvertBarcodeToString( char *szBarcode, int length, CString &strTemp, long lAsciiMode, long lRTC, char *currentDatenTime);
	void InitFormatControls();
	void ManipulateTimeStamp(CString& tmpRTC, CString& hour, CString& minute, CString& second, CString& ampm, CString& day, CString& month, CString& year);

	void ShowErrorMessage( long lErrorCode );
	void InitComboBox();
	void UpdateOpnListCtrl();
	void UpdateOpnListCtrl(long nComPort);
	void UpdateOpnListCtrlCounters();
	void AddToOpnListCtrl(SOpn200xStatus *m_itDev);
	SOpn200xStatus *AddDevice(long nComPort);
	void RemoveDevice(long nComPort);
	void RemoveAllDevices(void);
	RECT WindowRectangle(void);
	SOpn200xStatus* GetDeviceStatus(long nComPort);
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);

	void SetupMinimizeToTray();
	void MinimizeToTray();
	void MaximizeFromTray();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	static int __stdcall csp2PollCallback( long nComport );
	static int __stdcall csp2DeviceChangedCallback( long nComport );
	static void __stdcall COPN2001_DEMODlg::FirmwareProgressCallback(short Comport, short Percentage, short Status, LPCTSTR bstrStatusMsg);
	void HandleFirmwareProgressCallback(short Comport, short Percentage, short Status, LPCTSTR bstrStatusMsg);
	int HandlePollCallback( long nComPort );
	int HandleDeviceChangeCallback( long nComPort );
	int IsHandlingPollCallback(void);
	void UpdateDeviceStatus(SOpn200xStatus *Status, bool Add);
	void DoFirmwareUpdate();
	
	CComboBox m_cmboPort;
	afx_msg void OnCbnSelchangeCmboPort();
	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnClose();
	CButton m_btnOpen;
	CButton m_btnClose;
	CButton m_btnShowAll;
	afx_msg void OnBnClickedBtnInterrogate();
	CStatic m_statDllVersion;
	CStatic m_statOSVersion;
	CStatic m_statID;
	LRESULT OnDeviceChange( WPARAM wParam, LPARAM lParam );
	afx_msg void OnBnClickedBtnGetTime();
	afx_msg void OnBnClickedBtnSetTime();
	afx_msg void OnBnClickedCheckAuto();
	CButton m_btnAuto;
	CButton m_btnDetails;
	CListBox m_listBcdData;
	CListCtrl m_listOPNs;
	CFont m_font;
	afx_msg void OnBnClickedBtnGetBarcode();
	afx_msg void OnBnClickedBtnClearBarcode();
	afx_msg void OnBnClickedBtnParameters();
	afx_msg void OnBnClickedBtnParameters(long m_lComOpen, std::list<SOpn200xStatus*> m_lPortList);
	afx_msg void OnBnClickedCheckSave();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnAbout();
	afx_msg void OnBnClickedCheckSymbology();
	afx_msg void OnBnClickedCheckTime();
	afx_msg void OnBnClickedCheckDate();
	afx_msg void OnBnClickedRadioFormatText();
	afx_msg void OnBnClickedRadioFormatCSV();
	afx_msg void OnBnClickedRadioTime12();
	afx_msg void OnBnClickedRadioTime24();
	afx_msg void OnBnClickedRadioDateUSA();
	afx_msg void OnBnClickedRadioDateEU();
	afx_msg void OnBnClickedRadioDateLong();
	afx_msg void OnCbnSelChangeComboDelimeter();
	afx_msg void OnBnClickedShowHeader();
	
	//Text Formatting Controls
	CButton m_checkSymbology;
	CButton m_checkTime;
	CButton m_checkDate;
	CButton m_checkSerial;
	CButton m_radioText;
	CButton m_radioCSV;
	CComboBox m_cmboDelimeter;
	CButton m_radiot12;
	CButton m_radiot24;
	CButton m_radiodUSA;
	CButton m_radiodEU;
	CButton m_radiodLong;
	CButton m_checkCurrentTime;
	CButton m_checkCurrentDate;
	CButton m_checkShowHeader;
	CButton m_checkTrayIcon;
	//End Text Formatting Controls
//	afx_msg void OnCbnEditUpdateComboDelimeter();
	afx_msg void OnBnClickedBtnBrowse();
	afx_msg void OnBnClickedBtnBrowse2();
	afx_msg void OnBnClickedBtnFirmwareUpdate();
	afx_msg void OnBnClickedShowAll();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheckSerial();
	afx_msg void OnBnClickCheckCTime();
	afx_msg void OnBnClickedCurrentDate();
	afx_msg void OnBnClickedTrayIcon();
	afx_msg void OnBnClickedBtnViewDetails();
	afx_msg void OnNMDblclkListOpns(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListOpns(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeFirmwareFileEdit();
	afx_msg void OnNMRClickListOpns(NMHDR *pNMHDR, LRESULT *pResult);
};

