// OPN2001_DEMODlg.cpp : implementation file
//

#include "stdafx.h"
#include "OPN2001_DEMO.h"
#include "OPN2001_DEMODlg.h"
#include "csp2.h"		// csp2 dll header files
#include "SerialEnum.h" // serial port enumerator
#include <dbt.h>
#include <shlwapi.h>
#include "DlgParameters.h"
#include "INIFILE.h"
#include "Platform.h"
#include "FileVersionInfo.h"
#include "afxwin.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
bool firstLoad = true;
bool firstAutoLoad = true;
bool firstDownload = true;  //indicates whether this is the first time data has been downloaded into a particular file during this session.
							//firstDownload controls whether the optional header will be written out to a file.
bool firmwareUpdateEnabled = false;

int dsr = -1;
int cts = -1;

WCHAR szModulePath[ 1024 + 1];
WCHAR szFirmwarePath[ 1024 + 1];
CString strCurSavePath;
CString strCurSaveFileName;
CString gszFormattedDeviceID;
int ConnectedDevices = 0;

HANDLE	g_hStopWaitFirmwareUpdate = NULL;	// Stop the firmware update event thread waiter
HANDLE	g_hStopWaitPollCallback = NULL;		// Stop the poll callback event thread waiter


void *COPN2001_DEMODlg::pDlgObject;

using namespace std; // needed for list

    enum symbologie
    {
        Bookland = 0x16,
        Codabar = 0x02,
        Code_11 = 0x0c,
        Code_32 = 0x20,
        Code_128 = 0x03,
        Code_39 = 0x01,
        Code_39_Full_ASCII = 0x13,
        Code_93 = 0x07,
        Composite = 0x1d,
        Coupon = 0x17,
        D25 = 0x04,
        Data_Matrix = 0x1b,
        EAN_128 = 0x0f,
        EAN_13 = 0x0b,
        EAN_13_2 = 0x4b,
        EAN_13_5 = 0x8b,
        EAN_8 = 0x0a,
        EAN_8_2 = 0x4a,
        EAN_8_5 = 0x8a,
        IATA = 0x05,
        ISBT_128 = 0x19,
        ISBT_128_concatenated = 0x21,
        ITF = 0x06,
        Macro_PDF = 0x28,
        MSI = 0x0E,
        PDF_417 = 0x11,
        Postbar_Canada = 0x26,
        Postnet_US = 0x1e,
        Postal_Australia = 0x23,
        Postal_Japan = 0x22,
        Postal_UK = 0x27,
        QR_code = 0x1c,
        RSS_limited = 0x31,
        RSS_14 = 0x30,
        RSS_Expanded = 0x32,
        Signature = 0x24,
        Trioptic_Code_39 = 0x15,
        UPCA = 0x08,
        UPCA_2 = 0x48,
        UPCA_5 = 0x88,
        UPCE = 0x09,
        UPCE_2 = 0x49,
        UPCE_5 = 0x89,
        UPCE1 = 0x10,
        UPCE1_2 = 0x50,
        UPCE1_5 = 0x90
    };

	const SBCD_DEFS bcodedefs[] = 
	{
        L"Bookland",	Bookland,
        L"Codabar",		Codabar,
        L"Code-11",		Code_11,
		L"Code-32",		Code_32,
        L"Code-128",	Code_128,
		L"Code-39",		Code_39,
        L"Code-39 full ASCII", Code_39_Full_ASCII,
        L"Code-93",		Code_93,
        L"Composite",	Composite,
        L"Coupon",		Coupon,
        L"D 2 of 5",	D25,
        L"Data Matrix", Data_Matrix,
        L"EAN-128",		EAN_128,
        L"EAN-13",		EAN_13,
        L"EAN-13 + 2",	EAN_13_2,
        L"EAN-13 + 5",	EAN_13_5,
        L"EAN-8",		EAN_8,
        L"EAN-8 + 2",	EAN_8_2,
        L"EAN-8 + 5",	EAN_8_5,
        L"IATA",		IATA,
        L"ISBT-128",	ISBT_128,
        L"ISBT-128 concatenated", ISBT_128_concatenated,
        L"ITF",			ITF,
		L"Macro PDF",	Macro_PDF,
        L"MSI",			MSI,
        L"PDF-417",		PDF_417,
        L"Postbar (Canada)", Postbar_Canada,
        L"Postnet (US)",Postnet_US,
        L"Postal (Australia)",	Postal_Australia,
        L"Postal (Japan)",	Postal_Japan,
        L"Postal (UK)",	Postal_UK,
        L"QR-code",		QR_code,
        L"RSS-limited",	RSS_limited,
        L"RSS-14",		RSS_14,
        L"RSS-expanded",RSS_Expanded,
        L"Signature",	Signature,
        L"Tri-Optic",	Trioptic_Code_39,
        L"UPC-A",		UPCA,
        L"UPC-A + 2",	UPCA_2,
        L"UPC-A + 5",	UPCA_5,
        L"UPC-E",		UPCE,
        L"UPC-E + 2",	UPCE_2,
        L"UPC-E + 5",	UPCE_5,
        L"UPC-E1",		UPCE1,
        L"UPC-E1 + 2",	UPCE1_2,
        L"UPC-E1 + 5",	UPCE1_5
	};
#define MAX_BCD		( sizeof(bcodedefs) / sizeof(SBCD_DEFS))

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CStatic c_VersionName;
	CStatic c_ApplVersion;
	CStatic c_Csp2Version;
	CStatic c_LoadDllVersion;
	CStatic c_Copyright;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STAT_VERSION, c_VersionName);
	DDX_Control(pDX, IDC_STAT_APPL_VERSION, c_ApplVersion);
	DDX_Control(pDX, IDC_STAT_CSP2_VERSION, c_Csp2Version);
	DDX_Control(pDX, IDC_STAT_LOADDLL_VERSION, c_LoadDllVersion);
	DDX_Control(pDX, IDC_STAT_COPYRIGHT, c_Copyright);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char szDllVersion[25 + 1];
	CString strTemp;

	CFileVersionInfo  fvi;			// Change the version with the resource editor
	fvi.Create();

	strTemp.Format( _T("%s (%s)"), fvi.GetFileDescription(), fvi.GetProductVersion());
	c_VersionName.SetWindowText(strTemp);
	
	strTemp.Format( _T("Version Info: %s"), fvi.GetInternalName());
	c_ApplVersion.SetWindowText( strTemp );

	if( csp2GetDllVersion( szDllVersion, 25 ) != SETUP_ERROR)
	{
		strTemp.Format( _T("CSP2 DLL: %S"), szDllVersion );
		c_Csp2Version.SetWindowText( strTemp );
	}
	
	strTemp.Format( _T("Copyright %s"), fvi.GetLegalCopyright());
	c_Copyright.SetWindowText( strTemp );

	if( csp2GetLoadDllVersion( szDllVersion, 25 ) != SETUP_ERROR)
	{
		strTemp.Format( _T("LoadDLL: %S"), szDllVersion );
		c_LoadDllVersion.SetWindowText( strTemp );
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// COPN2001_DEMODlg dialog




COPN2001_DEMODlg::COPN2001_DEMODlg(CWnd* pParent /*=NULL*/)
	: CDialog(COPN2001_DEMODlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nUsedPort = 0; //	COM1
	WindowRect.right = 0;

	m_bAutoNotClicked = FALSE;
}

void COPN2001_DEMODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMBO_PORT, m_cmboPort);
	DDX_Control(pDX, IDC_BTN_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_SHOW_ALL, m_btnShowAll);
	DDX_Control(pDX, IDC_STAT_DLL_VERSION, m_statDllVersion);
	DDX_Control(pDX, IDC_STAT_OS_VERSION, m_statOSVersion);
	DDX_Control(pDX, IDC_STAT_ID, m_statID);
	DDX_Control(pDX, IDC_CHECK_AUTO, m_btnAuto);
	DDX_Control(pDX, IDC_LIST_BCD_DATA, m_listBcdData);
	DDX_Control(pDX, IDC_LIST_OPNS, m_listOPNs);
	DDX_Control(pDX, IDC_CHECK_SYMBOLOGY, m_checkSymbology);
	DDX_Control(pDX, IDC_CHECK_TIME, m_checkTime);
	DDX_Control(pDX, IDC_CHECK_DATE, m_checkDate);
	DDX_Control(pDX, IDC_CHECK_SERIAL, m_checkSerial);
	DDX_Control(pDX, IDC_RADIO_FORMAT_TEXT, m_radioText);
	DDX_Control(pDX, IDC_RADIO_FORMAT_CSV, m_radioCSV);
	DDX_Control(pDX, IDC_COMBO_DELIMETER, m_cmboDelimeter);
	DDX_Control(pDX, IDC_RADIO_TIME_12, m_radiot12);
	DDX_Control(pDX, IDC_RADIO_TIME_24, m_radiot24);
	DDX_Control(pDX, IDC_RADIO_DATE_USA, m_radiodUSA);
	DDX_Control(pDX, IDC_RADIO_DATE_EU, m_radiodEU);
	DDX_Control(pDX, IDC_RADIO_DATE_LONG, m_radiodLong);
	DDX_Control(pDX, IDC_CURRENT_TIME, m_checkCurrentTime);
	DDX_Control(pDX, IDC_CURRENT_DATE, m_checkCurrentDate);
	DDX_Control(pDX, IDC_SHOW_HEADER, m_checkShowHeader);
	DDX_Control(pDX, IDC_CHECK_TRAY_ICON, m_checkTrayIcon);
	DDX_Control(pDX, IDC_BTN_VIEW_DETAILS, m_btnDetails);
}
BEGIN_MESSAGE_MAP(COPN2001_DEMODlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_CMBO_PORT, &COPN2001_DEMODlg::OnCbnSelchangeCmboPort)
	ON_BN_CLICKED(IDC_BTN_OPEN, &COPN2001_DEMODlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &COPN2001_DEMODlg::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_INTERROGATE, &COPN2001_DEMODlg::OnBnClickedBtnInterrogate)
	ON_MESSAGE( WM_DEVICECHANGE, &COPN2001_DEMODlg::OnDeviceChange )
	ON_BN_CLICKED(IDC_BTN_GET_TIME, &COPN2001_DEMODlg::OnBnClickedBtnGetTime)
	ON_BN_CLICKED(IDC_BTN_SET_TIME, &COPN2001_DEMODlg::OnBnClickedBtnSetTime)
	ON_BN_CLICKED(IDC_CHECK_AUTO, &COPN2001_DEMODlg::OnBnClickedCheckAuto)
	ON_BN_CLICKED(IDC_BTN_GET_BARCODE, &COPN2001_DEMODlg::OnBnClickedBtnGetBarcode)
	ON_BN_CLICKED(IDC_BTN_CLEAR_BARCODE, &COPN2001_DEMODlg::OnBnClickedBtnClearBarcode)
	ON_BN_CLICKED(IDC_BTN_PARAMETERS, &COPN2001_DEMODlg::OnBnClickedBtnParameters)
	ON_BN_CLICKED(IDC_CHECK_SAVE, &COPN2001_DEMODlg::OnBnClickedCheckSave)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_ABOUT, &COPN2001_DEMODlg::OnBnClickedBtnAbout)
	ON_BN_CLICKED(IDC_CHECK_SYMBOLOGY, &COPN2001_DEMODlg::OnBnClickedCheckSymbology)
	ON_BN_CLICKED(IDC_CHECK_TIME, &COPN2001_DEMODlg::OnBnClickedCheckTime)
	ON_BN_CLICKED(IDC_CHECK_DATE, &COPN2001_DEMODlg::OnBnClickedCheckDate)
	ON_BN_CLICKED(IDC_RADIO_FORMAT_TEXT, &COPN2001_DEMODlg::OnBnClickedRadioFormatText)
	ON_BN_CLICKED(IDC_RADIO_FORMAT_CSV, &COPN2001_DEMODlg::OnBnClickedRadioFormatCSV)
	ON_BN_CLICKED(IDC_RADIO_TIME_12, &COPN2001_DEMODlg::OnBnClickedRadioTime12)
	ON_BN_CLICKED(IDC_RADIO_TIME_24, &COPN2001_DEMODlg::OnBnClickedRadioTime24)
	ON_BN_CLICKED(IDC_RADIO_DATE_USA, &COPN2001_DEMODlg::OnBnClickedRadioDateUSA)
	ON_BN_CLICKED(IDC_RADIO_DATE_EU, &COPN2001_DEMODlg::OnBnClickedRadioDateEU)
	ON_BN_CLICKED(IDC_RADIO_DATE_LONG, &COPN2001_DEMODlg::OnBnClickedRadioDateLong)
	ON_CBN_SELCHANGE(IDC_COMBO_DELIMETER, &COPN2001_DEMODlg::OnCbnSelChangeComboDelimeter)
//	ON_CBN_EDITUPDATE(IDC_COMBO_DELIMETER, &COPN2001_DEMODlg::OnCbnEditUpdateComboDelimeter)
	ON_BN_CLICKED(IDC_BTN_BROWSE, &COPN2001_DEMODlg::OnBnClickedBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_BROWSE2, &COPN2001_DEMODlg::OnBnClickedBtnBrowse2)
	ON_BN_CLICKED(IDC_SHOW_ALL, &COPN2001_DEMODlg::OnBnClickedShowAll)
	ON_BN_CLICKED(IDC_CHECK_SERIAL, &COPN2001_DEMODlg::OnBnClickedCheckSerial)
	ON_BN_CLICKED(IDC_CURRENT_TIME, &COPN2001_DEMODlg::OnBnClickCheckCTime)
	ON_BN_CLICKED(IDC_CURRENT_DATE, &COPN2001_DEMODlg::OnBnClickedCurrentDate)
	ON_BN_CLICKED(IDC_SHOW_HEADER, &COPN2001_DEMODlg::OnBnClickedShowHeader)
	ON_BN_CLICKED(IDC_CHECK_TRAY_ICON, &COPN2001_DEMODlg::OnBnClickedTrayIcon)
	ON_BN_CLICKED(IDC_BTN_VIEW_DETAILS, &COPN2001_DEMODlg::OnBnClickedBtnViewDetails)
	ON_MESSAGE(WM_TRAY_MESSAGE,OnTrayNotify)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPNS, &COPN2001_DEMODlg::OnNMDblclkListOpns)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_OPNS, &COPN2001_DEMODlg::OnLvnItemchangedListOpns)
	ON_EN_CHANGE(IDC_FIRMWARE_FILE_EDIT, &COPN2001_DEMODlg::OnEnChangeFirmwareFileEdit)
	ON_BN_CLICKED(IDC_BTN_FIRMWARE_UPDATE, &COPN2001_DEMODlg::OnBnClickedBtnFirmwareUpdate)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_OPNS, &COPN2001_DEMODlg::OnNMRClickListOpns)
END_MESSAGE_MAP()


// COPN2001_DEMODlg message handlers

BOOL COPN2001_DEMODlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString my_AppDataPath;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_font.CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");
	m_btnDetails.SetFont(&m_font);
	
	// TODO: Add extra initialization here
	pDlgObject = this;

	GetModuleFileName( AfxGetInstanceHandle(), szModulePath, 1024 );
	PathRemoveFileSpec( szModulePath );
	

	//m_strConfigFile = szModulePath;
	my_AppDataPath = GetAppDataPath();
	if( 0 == my_AppDataPath.Compare(platformError) )
	{
		m_strConfigFile = szModulePath;
		m_strConfigFile += _T("\\OPN_APPL.INI");
	}
	else
	{
		m_strConfigFile = my_AppDataPath;
		m_strConfigFile += _T("\\opticon");
		if(!DirectoryExists(m_strConfigFile))
		{
			if(CreateAppDataDirectory(m_strConfigFile))
			{
				m_strConfigFile += _T("\\OPN Companion Application");
				if(!DirectoryExists(m_strConfigFile))
				{
					if(CreateAppDataDirectory(m_strConfigFile))
					{
						m_strConfigFile += _T("\\OPN_APPL.INI");
					}
					else // failed to create or properly set permissions on one of the directories so set the path back to the exe runtime folder
					{
						m_strConfigFile = szModulePath;
						m_strConfigFile += _T("\\OPN_APPL.INI");
					}
				}
				else
				{
					m_strConfigFile += _T("\\OPN_APPL.INI");
				}
			}
			else // failed to create or properly set permissions on one of the directories so set the path back to the exe runtime folder
			{
				m_strConfigFile = szModulePath;
				m_strConfigFile += _T("\\OPN_APPL.INI");
			}
		}
		else
		{
			m_strConfigFile += _T("\\OPN Companion Application");
			if(!DirectoryExists(m_strConfigFile))
			{
				if(CreateAppDataDirectory(m_strConfigFile))
				{
					m_strConfigFile += _T("\\OPN_APPL.INI");
				}
				else // failed to create or properly set permissions on one of the directories so set the path back to the exe runtime folder
				{
					m_strConfigFile = szModulePath;
					m_strConfigFile += _T("\\OPN_APPL.INI");
				}
			}
			else
			{
				m_strConfigFile += _T("\\OPN_APPL.INI");
			}
		}
	}
	
	SetupMinimizeToTray();

	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );

	//m_nUsedPort = iniOpnDemo.ReadInt( L"GLOBAL", L"USED_PORT", 0); 
	m_lComOpen = -1;
	m_bSaveToFile = iniOpnDemo.ReadBool( L"GLOBAL", L"SAVEFILE", FALSE); 
	m_bAuto = iniOpnDemo.ReadBool( L"GLOBAL", L"AUTO", FALSE); 
	m_bShowAll = iniOpnDemo.ReadBool( L"GLOBAL", L"SHOWALL", FALSE); 
	m_bDetails = iniOpnDemo.ReadBool( L"GLOBAL", L"DETAILS", FALSE); 
	m_checkTrayIcon.SetCheck( (iniOpnDemo.ReadInt( L"GLOBAL", L"TRAY_ICON", FALSE)));
	m_btnDetails.SetWindowText( (m_bDetails) ? L"<<" : L">>" );

	InitComboBox();
	InitFormatControls();

	char szVersion[ 20 + 1];
	CString strVersion;
	csp2GetDllVersion( szVersion, 20 );
	strVersion.Format( _T("%S"), szVersion );
	m_statDllVersion.SetWindowText( strVersion );
	
	CButton *pBtn;
	
	pBtn = (CButton*)GetDlgItem( IDC_CHECK_SAVE );
	pBtn->SetCheck( m_bSaveToFile );

	m_btnShowAll.SetCheck( m_bShowAll );
	m_btnShowAll.EnableWindow( FALSE );
	m_btnDetails.EnableWindow( FALSE );

	m_btnAuto.SetCheck( m_bAuto );

	m_listOPNs.SetExtendedStyle(m_listOPNs.GetExtendedStyle() | /*LVS_EX_CHECKBOXES*/ LVS_EX_FULLROWSELECT);

	m_listOPNs.InsertColumn(0, _T("Port"), 0, 58, 0);
	m_listOPNs.InsertColumn(1, _T("Serial"), 0, 58, 1);
	m_listOPNs.InsertColumn(2, _T("Version"), 0, 70, 2);
	m_listOPNs.InsertColumn(3, _T("Barcodes"), 0, 58, 3);
	m_listOPNs.InsertColumn(4, _T("Status"), 0, 100, 4);

	UpdateOpnListCtrl();
	
	csp2SetRetryCount(3);

	csp2SetDefaultTimeout( iniOpnDemo.ReadInt( L"GLOBAL", L"TIMEOUT", 1000 ) );

#ifdef _DEBUG
	csp2SetDebugMode(2); // Enable debug
#endif

	if( m_nUsedPort != 0 ) //Added so open will be attempted when a port number is read from the ini file which indicates not a first run.
	{
		firstLoad = false;
	}

	if(m_bAuto == false)
	{
		// Now try to open the port
		OnBnClickedBtnOpen();
	}
	else
	{
		m_bAutoNotClicked = TRUE;
		OnBnClickedCheckAuto();
		m_bAutoNotClicked = FALSE;
	}

	firstAutoLoad = false;

	SetTimer(NULL, 500, NULL);

	if(m_lComOpen == -1 && m_bShowAll == FALSE)		// If no OPN2001 is connected by USB or the comport could not be opened
	{												// make sure that the 'show all' option is available.
		m_btnShowAll.EnableWindow( TRUE );
	}

	//Create the ToolTip control
	if( !m_ToolTip.Create(this))
	{
	   TRACE0("Unable to create the ToolTip!");
	}
	else
	{
	  // Add tool tips to the controls, either by hard coded string 
	  // or using the string table resource
	  CButton *pBtn = (CButton*)GetDlgItem( IDC_BTN_BROWSE );
	  m_ToolTip.AddTool( pBtn, _T("Hint: Use #SERIAL#.txt to create 1 file per device"));
	  m_ToolTip.Activate(TRUE);
	}

#ifdef WIN64_DLL
	this->SetWindowTextW(L"OPN2001(x64)");
#endif
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COPN2001_DEMODlg::InitComboBox()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );

	//
	// First find all the available serial ports
	//
	CSerialEnum::GetSerialPorts( m_nPortArray );

	//
	// Remove incompatible devices
	//
	m_itPorts = m_nPortArray.begin();

	while (m_itPorts != m_nPortArray.end())
	{
		if( csp2IsOpnCompatiblePort(*m_itPorts) == false && !m_bShowAll )
			m_itPorts = m_nPortArray.erase( m_itPorts );
		else
			m_itPorts++;
	}

	m_nUsedPort = iniOpnDemo.ReadInt( L"GLOBAL", L"USED_PORT", 0); 
	m_nPortArray.sort();
	// Fill the combobox
	m_cmboPort.ResetContent(); // Clear the list in the combobox;
	CString strPort;
	int x;
	for( m_itPorts = m_nPortArray.begin(), x = 0; m_itPorts != m_nPortArray.end(); m_itPorts++, x++ )
	{
		strPort.Format(_T("COM%d"), *m_itPorts );
		m_cmboPort.AddString( strPort );
		m_cmboPort.SetItemData( x, *m_itPorts );

		AddDevice(*m_itPorts);
	}
	if( m_nUsedPort == 0 )
		m_cmboPort.SetCurSel( 0 );
	else
	{
		strPort.Format( L"COM%d", m_nUsedPort );
		x = m_cmboPort.FindString( 0, strPort );
		if(x != -1)
			m_cmboPort.SetCurSel( x );
		else
			m_cmboPort.SetCurSel( 0 );
	}


	OnCbnSelchangeCmboPort(); // Set the m_nUsedPort value
}

void COPN2001_DEMODlg::UpdateOpnListCtrl(long nComPort)
{
	std::list<SOpn200xStatus*>::iterator m_itDev;

	ConnectedDevices = 0;

	if(m_DeviceArray.size() == 0)
		return;

	for(m_itDev = m_DeviceArray.begin();m_itDev != m_DeviceArray.end(); m_itDev++)
	{
		SOpn200xStatus *dev = *m_itDev;

		if(nComPort == dev->nComPort)
		{
			UpdateDeviceStatus(dev, false);
		}
		
		if(dev->iConnected)
			++ConnectedDevices;
    }

	UpdateOpnListCtrlCounters();
}

void COPN2001_DEMODlg::UpdateDeviceStatus(SOpn200xStatus *Status, bool Add)
{
	CString strCount;
		
	if(Status == NULL)
		return;

	int index = Status->index;

	if(Add)
	{
		LVITEM lvi;
		CString strPort;

		strPort.Format(_T("COM%d"), Status->nComPort );
		
		// Insert the first item
		lvi.mask =  LVIF_TEXT;

		for(int column=0; column<4; ++column)
		{
			lvi.iItem = index;
			lvi.iSubItem = column;

			if(column == 0)
			{
				lvi.pszText = (LPTSTR)(LPCTSTR)(strPort);
				m_listOPNs.InsertItem(&lvi);
			}
			else
			{
				lvi.pszText = (_T(""));
				m_listOPNs.SetItem(&lvi);
			}
		}
	}

	m_listOPNs.SetItemText(index, 1, (LPTSTR)(LPCTSTR)(Status->sDeviceId));
	m_listOPNs.SetItemText(index, 2, (LPTSTR)(LPCTSTR)(Status->sSwVersion));
			
	if(Status->nReadBarcodes >= 0)
		strCount.Format(_T("%d"), Status->nReadBarcodes );
	else
		strCount.Format(_T(""));

	m_listOPNs.SetItemText(index, 3, strCount);

	switch(Status->iConnected)
	{
		case TRUE:
			m_listOPNs.SetItemText(index, 4, _T("Connected"));
			break;

		case FIRMWARE_UPDATE:
			m_listOPNs.SetItemText(index, 4, _T("Firmware update"));
			break;

		case FIRMWARE_UPDATE_FAILED:
			m_listOPNs.SetItemText(index, 4, _T("Firmware failed"));
			break;

		default:
			m_listOPNs.SetItemText(index, 4, _T(""));
			break;
	}

	m_listOPNs.Update(index);
}

void COPN2001_DEMODlg::AddToOpnListCtrl(SOpn200xStatus *Status)
{
	if(Status == NULL)
		return;

	UpdateDeviceStatus(Status, true);

	UpdateOpnListCtrlCounters();
}

void COPN2001_DEMODlg::UpdateOpnListCtrlCounters()
{
	CStatic *pLabel = (CStatic *)GetDlgItem( IDC_COUNTERS );
	CString strCounters;

	strCounters.Format(_T("%d/%d"), ConnectedDevices, m_DeviceArray.size());
	pLabel->SetWindowTextW(strCounters);
}

void COPN2001_DEMODlg::UpdateOpnListCtrl()
{
	// First find the available serial ports
	std::list<SOpn200xStatus*>::iterator m_itDev;
	
	m_listOPNs.DeleteAllItems();

	ConnectedDevices = 0;

	if(m_DeviceArray.size() != 0)
	{
		for(m_itDev = m_DeviceArray.begin();m_itDev != m_DeviceArray.end(); m_itDev++)
		{
			SOpn200xStatus *dev = *m_itDev;
			
			UpdateDeviceStatus(dev, true);
		}
	}

	m_listOPNs.Invalidate(TRUE);

	UpdateOpnListCtrlCounters();
}

SOpn200xStatus *COPN2001_DEMODlg::AddDevice(long nComPort)
{
	SOpn200xStatus *newDevice = new SOpn200xStatus();

	newDevice->nComPort = nComPort;
	newDevice->nReadBarcodes = -1;
	newDevice->iConnected = FALSE;
	newDevice->index = (int)m_DeviceArray.size();		// Used as quick reference of the position in m_DeviceArray

	m_DeviceArray.push_back(newDevice);

	return newDevice;
}

SOpn200xStatus* COPN2001_DEMODlg::GetDeviceStatus(long nComPort)
{
	std::list<SOpn200xStatus*>::iterator m_itDev;

	for(m_itDev = m_DeviceArray.begin();m_itDev != m_DeviceArray.end(); m_itDev++)
	{
		SOpn200xStatus *dev = *m_itDev;

		if(dev->nComPort == nComPort)
		{
			return dev;
		}
	}
	return NULL;
}


void COPN2001_DEMODlg::RemoveDevice(long nComPort)
{
	std::list<SOpn200xStatus*>::iterator m_itDev;

	for(m_itDev = m_DeviceArray.begin();m_itDev != m_DeviceArray.end(); m_itDev++)
	{
		SOpn200xStatus *dev = *m_itDev;

		if(dev->nComPort == nComPort)
		{
			delete (*m_itDev);
			(*m_itDev) = NULL;
			m_DeviceArray.remove(*m_itDev);
			break;
		}
	}
}

void COPN2001_DEMODlg::RemoveAllDevices(void)
{
	if(m_DeviceArray.size() == 0)
		return;

	std::list<SOpn200xStatus*>::iterator m_itDev;

	for(m_itDev = m_DeviceArray.begin();m_itDev != m_DeviceArray.end(); ++m_itDev)
	{
		delete (*m_itDev);
		(*m_itDev) = NULL;
	}

	m_DeviceArray.clear();
}


void COPN2001_DEMODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COPN2001_DEMODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COPN2001_DEMODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COPN2001_DEMODlg::OnCbnSelchangeCmboPort()
{
	int nSelected = m_cmboPort.GetCurSel();

	if( nSelected == -1 )
		return;
	
	m_nUsedPort = (int) m_cmboPort.GetItemData( nSelected );
	
	if( m_nUsedPort > 0 )
	{
		OnBnClickedBtnClose();
		OnBnClickedBtnOpen();
	}
}

void COPN2001_DEMODlg::OnBnClickedBtnOpen()
{
	COPN2001_DEMODlg* pDlg = this;

	dsr = -1;
	cts = -1;

	pDlg->SetWindowPos(NULL, 0, 0, WindowRectangle().right / 2, WindowRectangle().bottom, SWP_NOZORDER|SWP_NOMOVE);

	if(firstLoad)
	{
		firstLoad = false;
		return;
	}	

	if(m_cmboPort.GetCount() == 0)
	{
		//Try detecting device just one more time
		::MessageBox(m_hWnd,_T("No device detected or incompatible device connected.\r\nPlease click \'OK\' and connect your OPN-2001."),_T("Error"),MB_OK|MB_ICONERROR);
		return;
	}
		
	m_btnOpen.EnableWindow( FALSE );
	m_btnClose.EnableWindow( TRUE );
	m_cmboPort.EnableWindow( FALSE );
	m_btnShowAll.EnableWindow( FALSE );
	m_btnDetails.EnableWindow( FALSE );
	
	// Store the settings in the INI file
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"GLOBAL", L"USED_PORT", m_nUsedPort); 
	
	CButton* pBtn;

	pBtn = (CButton*)GetDlgItem( IDC_BTN_INTERROGATE );
	pBtn->EnableWindow( TRUE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_GET_TIME );
	pBtn->EnableWindow( TRUE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_SET_TIME );
	pBtn->EnableWindow( TRUE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_GET_BARCODE );
	pBtn->EnableWindow( TRUE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_CLEAR_BARCODE );
	pBtn->EnableWindow( TRUE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_PARAMETERS );
	pBtn->EnableWindow( TRUE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_FIRMWARE_UPDATE );
	pBtn->EnableWindow( TRUE );
	pBtn = (CButton*)GetDlgItem( IDC_CHECK_SAVE );
	pBtn->EnableWindow( TRUE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_BROWSE );
	pBtn->EnableWindow( TRUE );
	

	int x = m_cmboPort.GetCurSel();

	if( x != -1 )
	{
		m_lComOpen = (long)m_cmboPort.GetItemData(x);

		if(csp2InitEx(m_lComOpen) != STATUS_OK)
		{
			m_lComOpen = -1;
			::MessageBox( m_hWnd, _T("Error opening serial port"), _T("Error"), MB_OK | MB_ICONERROR );
		}
	}
}

void COPN2001_DEMODlg::OnBnClickedBtnClose()
{
	CButton* pBtn;
	if(m_lComOpen != -1L)
		csp2RestoreEx(m_lComOpen);
	m_btnOpen.EnableWindow();
	m_cmboPort.EnableWindow();

	m_lComOpen = -1;

	CStatic* px;
	px = (CStatic*)GetDlgItem(IDC_DSR_STATUS);
	px->SetBitmap(::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_DISABLED)));

	px = (CStatic*)GetDlgItem(IDC_DATA_STATUS);
	px->SetBitmap(::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_DISABLED)));

	pBtn = (CButton*)GetDlgItem( IDC_BTN_INTERROGATE );
	pBtn->EnableWindow( FALSE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_GET_TIME );
	pBtn->EnableWindow( FALSE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_SET_TIME );
	pBtn->EnableWindow( FALSE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_GET_BARCODE );
	pBtn->EnableWindow( FALSE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_CLEAR_BARCODE );
	pBtn->EnableWindow( FALSE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_PARAMETERS );
	pBtn->EnableWindow( FALSE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_FIRMWARE_UPDATE );
	pBtn->EnableWindow( FALSE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_CLOSE );
	pBtn->EnableWindow( FALSE );
	pBtn = (CButton*)GetDlgItem( IDC_BTN_BROWSE );
	pBtn->EnableWindow( FALSE );
	pBtn = (CButton*)GetDlgItem( IDC_SHOW_ALL );
	pBtn->EnableWindow( TRUE );
}


void COPN2001_DEMODlg::OnBnClickedBtnInterrogate()
{
	long lRet;
	char szSwVersion[ 9 + 1 ];
	unsigned char szDeviceID[ 9 + 1 ];
	CString strTemp;
	
	lRet = csp2Interrogate();
	if( lRet != STATUS_OK )
	{
		ShowErrorMessage( lRet );
		return;

	}

	csp2GetDeviceId( (char*)szDeviceID, 9);

	// szDeviceID need now to be converted to a real number
	strTemp.Format( _T("%02X%02X%02X"), szDeviceID[ 5 ] & 0xFF, 
										szDeviceID[ 6 ] & 0xFF,
										szDeviceID[ 7 ] & 0xFF);
	m_statID.SetWindowText( strTemp );
	
	csp2GetSwVersion( szSwVersion, 9 );
	strTemp.Format(_T("%S"), szSwVersion );
	m_statOSVersion.SetWindowText( strTemp );
}


//PDEV_BROADCAST_DEVICEINTERFACE 
LRESULT COPN2001_DEMODlg::OnDeviceChange( WPARAM wParam, LPARAM lParam )
{
	 if(m_btnAuto.GetCheck() == TRUE)	// No need to handle device change event while in automatic mode, beause CSP2.dll handles this
		 return FALSE;
	 

	 if( DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam ) 
	 {
        PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
        switch( pHdr->dbch_devicetype ) 
		{
			case DBT_DEVTYP_PORT:
				PDEV_BROADCAST_PORT pDevPort = (PDEV_BROADCAST_PORT)pHdr;
				if( wcsncmp( pDevPort->dbcp_name, L"COM", 3 ) == 0 )
				{
					// Get the port number
					int nComPort = _wtoi( pDevPort->dbcp_name + 3 );
				
					if( DBT_DEVICEARRIVAL == wParam )
					{
						//check if it is OPN2001 compatible device
						if(csp2IsOpnCompatiblePort(nComPort) || m_bShowAll)
						{
							m_nPortArray.push_back( nComPort ); // add the new port 
							m_nPortArray.sort();				// sort the list
							m_nPortArray.unique();				// all items must be unique
							
							if(GetDeviceStatus(nComPort) == NULL)
							{
								AddDevice(nComPort);
							}

							CString strTemp;
							m_itPorts = m_nPortArray.begin();
							for(int x = 0; m_itPorts != m_nPortArray.end(); x++, m_itPorts++ )	// This code keeps the combobox sorted
							{
								if( *m_itPorts != m_cmboPort.GetItemData( x ) )
								{
									strTemp.Format( L"COM%d", *m_itPorts );
									m_cmboPort.InsertString( x, strTemp );
									m_cmboPort.SetItemData( x, *m_itPorts );
								}
							}

							int index =  m_cmboPort.FindString( 0, pDevPort->dbcp_name );	// Select the newly inserted device
							
							m_cmboPort.SetCurSel(index);
							OnCbnSelchangeCmboPort();
						}
						else
						{
							// remove the item from the list
							m_nPortArray.remove( nComPort );
							int index =  m_cmboPort.FindString( 0, pDevPort->dbcp_name );
							m_cmboPort.DeleteString(index);
							RemoveDevice(nComPort);
						}
					}
					else
					{
						// remove the item from the list
						m_nPortArray.remove( nComPort );
						int index =  m_cmboPort.FindString( 0, pDevPort->dbcp_name );
						m_cmboPort.DeleteString(index);

						if(m_cmboPort.GetCurSel()== CB_ERR && m_cmboPort.GetCount() > 0 && !m_bShowAll)
						{
							m_cmboPort.SetCurSel(0);
							OnCbnSelchangeCmboPort();
						}

						RemoveDevice(nComPort);
					}
			}
            break;
        }
    }
	return 0;
}

void COPN2001_DEMODlg::OnBnClickedBtnGetTime()
{
	//unsigned char buf[10];
	char buf[10];
	CString strTime;
	CString strDate;
	CString strTemp;
	long lRet;

	if( (lRet = csp2GetTime( (unsigned char *)buf )) == STATUS_OK )
	{
		
		//strTemp.Format( L"%d:%02d:%02d \r\n%d/%0d/%02d", buf[ 2 ],  buf[ 1 ], buf[ 0 ], buf[ 4 ], buf[ 3 ], buf[ 5 ]);
		UnpackCurrentTime(buf,strTime);
		UnpackCurrentDate(buf,strDate);
		strTemp = strTime + _T(", ")  + strDate;
		::MessageBox( m_hWnd, strTemp, L"Information", MB_OK | MB_ICONINFORMATION );
	}
	else
	{
		ShowErrorMessage( lRet );
	}
}


void COPN2001_DEMODlg::OnBnClickedBtnSetTime()
{
	unsigned char buf[10];
	SYSTEMTIME sysTime;
	long lRet;

	GetLocalTime( &sysTime );

	buf[0] = (unsigned char) sysTime.wSecond;	// seconds
	buf[1] = (unsigned char) sysTime.wMinute;	// minutes
	buf[2] = (unsigned char) sysTime.wHour;		// hours

	buf[3] = (unsigned char) sysTime.wDay;		// day
	buf[4] = (unsigned char) sysTime.wMonth;	// Month
	buf[5] = (unsigned char) (sysTime.wYear % 100 ); // year

	if(( lRet = csp2SetTime( buf )) == STATUS_OK )
	{
		::MessageBox( m_hWnd, L"Time is set", L"Information", MB_OK | MB_ICONINFORMATION );
	}
	else
	{
		ShowErrorMessage( lRet );
	}
}

void COPN2001_DEMODlg::OnBnClickedCheckAuto()
{
	CButton* pBtn;
	long PortArray[100];

	if( m_btnAuto.GetCheck() == TRUE )
	{
		if( !m_bAutoNotClicked )
		{
			OnBnClickedBtnClose();

			if( MessageBox( L"Auto mode will automatically retrieve all data from all OPN-2001s and simultaniously delete it from the devices. This occurs immediately upon connection of an OPN-2001 or at any time data is present while connected. Data will be saved to file.\r\n\r\nDo you wish to proceed?", L"Warning", MB_YESNOCANCEL  | MB_ICONWARNING | MB_TOPMOST ) != IDYES )
			{
				m_btnAuto.SetCheck( FALSE );
				return;
			}
		}

		RemoveAllDevices();

		int x, nCount = csp2GetOpnCompatiblePorts(PortArray);

		for(x=0; x<nCount; ++x)
			AddDevice(PortArray[x]);
		
		UpdateOpnListCtrl();

		// disable the other button options
		pBtn = (CButton*)GetDlgItem( IDC_BTN_INTERROGATE );
		pBtn->EnableWindow( FALSE );
		pBtn = (CButton*)GetDlgItem( IDC_BTN_GET_TIME );
		pBtn->EnableWindow( FALSE );
		pBtn = (CButton*)GetDlgItem( IDC_BTN_SET_TIME );
		pBtn->EnableWindow( FALSE );
		pBtn = (CButton*)GetDlgItem( IDC_BTN_GET_BARCODE );
		pBtn->EnableWindow( FALSE );
		pBtn = (CButton*)GetDlgItem( IDC_BTN_CLEAR_BARCODE );
		pBtn->EnableWindow( FALSE );
		pBtn = (CButton*)GetDlgItem( IDC_BTN_PARAMETERS );
		pBtn->EnableWindow( FALSE );
		pBtn = (CButton*)GetDlgItem( IDC_BTN_FIRMWARE_UPDATE );
		pBtn->EnableWindow( FALSE );
		pBtn = (CButton*)GetDlgItem( IDC_CHECK_SAVE );
		pBtn->EnableWindow( FALSE );
		pBtn = (CButton*)GetDlgItem( IDC_BTN_BROWSE );
		pBtn->EnableWindow( FALSE );

		if(m_bDetails == FALSE)
			this->SetWindowPos(NULL, 0, 0,  WindowRectangle().right / 2,  WindowRectangle().bottom, SWP_NOZORDER|SWP_NOMOVE);
		else
			this->SetWindowPos(NULL, 0, 0,  WindowRectangle().right,  WindowRectangle().bottom, SWP_NOZORDER|SWP_NOMOVE);

		m_cmboPort.EnableWindow(false);
		m_btnOpen.EnableWindow(false);
		m_btnClose.EnableWindow(false);
		m_btnShowAll.EnableWindow(false);
		m_btnDetails.EnableWindow(true);

		// enable automatic download mode
		if( !StartAutomaticDownload())
		{
			m_btnAuto.SetCheck( FALSE );
			this->SetWindowPos(NULL, 0, 0,  WindowRectangle().right / 2,  WindowRectangle().bottom, SWP_NOZORDER|SWP_NOMOVE);
		}
	}
	else
	{
		if(IsHandlingPollCallback())	// Ignore uncheck during poll callback to avoid huge crash
		{
			WaitForSingleObject( g_hStopWaitPollCallback, 5000UL );
		}

		// disable automatic download mode
		StopAutomaticDownload();

		OnBnClickedBtnOpen();
	}
	if( !m_bAutoNotClicked )
	{
		CIniFile iniOpnDemo;
		iniOpnDemo.SetPathName( m_strConfigFile );
		iniOpnDemo.WriteBool( L"GLOBAL", L"AUTO", m_btnAuto.GetCheck()); 
	}
}

BOOL COPN2001_DEMODlg::StartAutomaticDownload( void )
{
	long lRet;

	if( (lRet = csp2EnablePolling()) != STATUS_OK)
	{
		int (FAR WINAPI *pPollCallback)(long) = reinterpret_cast<int (FAR WINAPI *)(long)>( &csp2PollCallback );

		lRet = csp2StartPollingAll( pPollCallback );
	
		return ( lRet == STATUS_OK );
	}

	return ( lRet == STATUS_OK );
}

BOOL COPN2001_DEMODlg::StopAutomaticDownload( void )
{
	 //long lRet = csp2StopPolling();
	 long lRet = csp2DisablePolling();

	 return ( lRet == STATUS_OK );
}

int __stdcall COPN2001_DEMODlg::csp2DeviceChangedCallback(long nComport)
{
	COPN2001_DEMODlg *pDlg = (COPN2001_DEMODlg*) pDlgObject;
	return pDlg->HandleDeviceChangeCallback(nComport);
}

int __stdcall COPN2001_DEMODlg::csp2PollCallback(long nComport)
{
	int lRet;
	
	COPN2001_DEMODlg *pDlg = (COPN2001_DEMODlg*) pDlgObject;
	
	lRet = pDlg->HandlePollCallback(nComport);

	// Kick the PollingThreadFunc wait on my exit. 
    if( g_hStopWaitPollCallback != NULL) 
		SetEvent( g_hStopWaitPollCallback ); 

	return lRet;
}
void COPN2001_DEMODlg::UnpackCurrentTime( char *packedTimeandDate, CString &unpackedTime)
{
	//takes a csp2-style packed Time and Date buffer, and unpacks the time. Returns a time in human-readable form according to selected radio buttons.

	if(m_radiot24.GetCheck() == 1)
	{//don't need to do any data manipulation. Just pass it through.
		unpackedTime.Format( L"%d:%02d:%02d", packedTimeandDate[2],packedTimeandDate[1],packedTimeandDate[0]);
	}
	else if(m_radiot12.GetCheck() == 1)
	{
		short hours = 0;
		short minutes = 0;
		short seconds = 0;
		CString ampm;
		ampm.Format(L"AM");

		if(packedTimeandDate[2] < 13)
		{
			hours = packedTimeandDate[2];
			minutes = packedTimeandDate[1];
			seconds = packedTimeandDate[0];
		}
		else//hours greater than or equal to 13.
		{
			hours = packedTimeandDate[2] - 12;
			minutes = packedTimeandDate[1];
			seconds = packedTimeandDate[0];
			ampm.Format(L"PM");
		}
		unpackedTime.Format( L"%d:%02d:%02d %s", hours,minutes,seconds,ampm);
	}
}

void COPN2001_DEMODlg::UnpackCurrentDate( char *packedTimeandDate, CString &unpackedDate)
{
	//takes a csp2-style packed Time and Date buffer, and unpacks the date according to the radio buttons. Returns a date in human-readable form.
	
	short month = packedTimeandDate[4];
	short day =   packedTimeandDate[3];
	short year =  packedTimeandDate[5];

	if(m_radiodUSA.GetCheck() == 1)//MM/DD/YY
	{
		unpackedDate.Format(L"%02d/%02d/%02d", month, day, year);
	}
	else if(m_radiodEU.GetCheck() == 1)//DD/MM/YY
	{
		unpackedDate.Format(L"%02d/%02d/%02d", day, month, year);
	}
	else if(m_radiodLong.GetCheck() == 1)//YYYY-MM-DD
	{//realistically we'll never need more than two digits of precision, and it's safe to say that current dates have years after 2000.
		unpackedDate.Format(L"20%02d-%02d-%02d", year, month, day);
	}
}
void COPN2001_DEMODlg::GenerateHeader( CString &strTemp )
{//generates the header for the list of scans.
	CString wrapper;
	CString delimeter;

	if(m_radioCSV.GetCheck() == TRUE)
	{
		wrapper = "\"";
		
		switch(m_cmboDelimeter.GetCurSel())
		{
			case 0:
				delimeter = ",";
				break;

			case 1:
				delimeter = ";";
				break;
		}
	} 
	else 
	{
		wrapper = "";

		switch(m_cmboDelimeter.GetCurSel())
		{
			case 0:
				delimeter = ", ";
				break;

			case 1:
				delimeter = "; ";
				break;

			case 2:
				delimeter = "\t";
				break;
		}
	}

		strTemp.Append(wrapper);
		strTemp.AppendFormat(L"Barcode");
		strTemp.Append(wrapper);

		if(m_checkSymbology.GetCheck()) {
			strTemp.Append(delimeter);
			strTemp.Append(wrapper);
			strTemp.AppendFormat(L"Symbology");
			strTemp.Append(wrapper);
		}

		if(m_checkTime.GetCheck()) {
			strTemp.Append(delimeter);
			strTemp.Append(wrapper);
			strTemp.AppendFormat(L"Scan Time");
			strTemp.Append(wrapper);
		}

		if(m_checkDate.GetCheck()) {
			strTemp.Append(delimeter);
			strTemp.Append(wrapper);
			strTemp.AppendFormat(L"Scan Date");
			strTemp.Append(wrapper);
		}

		if(m_checkCurrentTime.GetCheck()) {

			strTemp.Append(delimeter);
			strTemp.Append(wrapper);
			strTemp.AppendFormat(L"Time Now");
			strTemp.Append(wrapper);
		}

		if(m_checkCurrentDate.GetCheck()) {

			strTemp.Append(delimeter);
			strTemp.Append(wrapper);
			strTemp.AppendFormat(L"Date Now");
			strTemp.Append(wrapper);

		}

		if(m_checkSerial.GetCheck()) {

			strTemp.Append(delimeter);
			strTemp.Append(wrapper);
			strTemp.AppendFormat(L"S/N");
			strTemp.Append(wrapper);

		}
}
void COPN2001_DEMODlg::ConvertBarcodeToString( char *szBarcode, int length, CString &strTemp, long lAsciiMode, long lRTC, char *currentDatenTime)
{// converts packed barcode data to a string.
	int nBcdLength;
	int nBarcodeType;
	char szTimeStamp[ 30 + 1]	= {0};
	//char ucTime[6] = {0};
	CString wrapper;
	CString delimeter;
	CString time;
	CString date;
	CString tmpRTC;

	CString hour;
	CString minute;
	CString second;
	CString ampm;
	CString day;
	CString month;
	CString year;

	if(m_radioCSV.GetCheck() == TRUE)
	{
		wrapper = "\"";
		
		switch(m_cmboDelimeter.GetCurSel())
		{
			case 0:
				delimeter = ",";
				break;

			case 1:
				delimeter = ";";
				break;
		}
	} 
	else 
	{
		wrapper = "";

		switch(m_cmboDelimeter.GetCurSel())
		{
			case 0:
				delimeter = ", ";
				break;

			case 1:
				delimeter = "; ";
				break;

			case 2:
				delimeter = "\t";
				break;
		}
	}

	nBarcodeType = (long)(szBarcode[ 1 ] & 0x000000FF);
	// Clear the ouput string;
	strTemp.Empty();

	if( ( length > 0 )&& ( lAsciiMode == PARAM_ON))
	{
		nBcdLength = length + 1 - ((lRTC == PARAM_ON)?6:2);
		
		// Store the barcode
		strTemp = wrapper;
		strTemp.AppendFormat( L"%*.*S", nBcdLength, nBcdLength, szBarcode + 2 );
		strTemp.Append(wrapper);

		// Append the barcode type
		if(m_checkSymbology.GetCheck() == TRUE)
		{
			int nIndex;
			for( nIndex = 0; nIndex < MAX_BCD; nIndex++ )
			{
				if( bcodedefs[ nIndex ].lSymbol == (long) nBarcodeType )
				{
					strTemp.Append(delimeter);
					strTemp.Append(wrapper);
					strTemp.AppendFormat( L"%s", bcodedefs[ nIndex ].szSymbolName );
					strTemp.Append(wrapper);
					break;
				}
			}

			if( nIndex == MAX_BCD )
			{
				strTemp.Append(delimeter);
				strTemp.Append(wrapper);
				strTemp.Append( L"Unknown Symbology" );
				strTemp.Append(wrapper);
			}
		}
		
		// Append timestamp of scan time if available
		if( (m_checkDate.GetCheck() == TRUE) || (m_checkTime.GetCheck() == TRUE) )
		{
			if( lRTC == PARAM_ON)
			{
				csp2TimeStamp2Str( (unsigned char *) szBarcode + ( length - 3 ), szTimeStamp, sizeof( szTimeStamp ));
				tmpRTC.Format( L"%S", szTimeStamp );
				ManipulateTimeStamp(tmpRTC, hour, minute, second, ampm, day, month, year);

				if(m_checkTime.GetCheck() == 1)
				{
					strTemp.Append(delimeter);
					strTemp.Append(wrapper);
					if(m_radiot12.GetCheck() == 1)
					{
						strTemp.AppendFormat(L"%s:%s:%s %s", hour, minute, second, ampm);
					}
					else if(m_radiot24.GetCheck() == 1)
					{
						int tmpHour = _ttoi(hour);
						if(ampm.Mid(0, 1) == 'P')
						{
							tmpHour += 12;
							if(tmpHour == 24)
							{
								hour = "12";
							}
							else
							{
								hour.Format(L"%d", tmpHour);
							}
						}
						else if( (ampm.Mid(0, 1) == 'A') && (tmpHour == 12) )
						{
							hour = "00";
						}
						strTemp.AppendFormat(L"%s:%s:%s", hour, minute, second);
					}
					strTemp.Append(wrapper);
				}

				if(m_checkDate.GetCheck() == 1)
				{
					strTemp.Append(delimeter);
					strTemp.Append(wrapper);
					if(m_radiodUSA.GetCheck() == 1)
					{
						strTemp.AppendFormat(L"%s/%s/%s", month, day, year);
					}
					else if(m_radiodEU.GetCheck() == 1)
					{
						strTemp.AppendFormat(L"%s/%s/%s", day, month, year);
					}
					else if(m_radiodLong.GetCheck() == 1)
					{
						strTemp.AppendFormat(L"20%s-%s-%s", year, month, day);
					}
					strTemp.Append(wrapper);
				}
			
				//strTemp.Append(delimeter);
				//strTemp.Append(wrapper);
				//strTemp.Append(tmpRTC);
				//strTemp.AppendFormat( L"%S", szTimeStamp );
				//strTemp.Append(wrapper);
			} 
			else 
			{
				strTemp.Append(delimeter);
				strTemp.Append(wrapper);
				strTemp.AppendFormat(L"%S", L"Real Time Clock (RTC) Disabled");
				strTemp.Append(wrapper);
			}
		}

		if( (m_checkCurrentDate.GetCheck() == TRUE) || (m_checkCurrentTime.GetCheck() == TRUE) )
		{
			if( lRTC == PARAM_ON)
			{
				CString emptyBuffer;
				if(m_checkCurrentTime.GetCheck() == 1)
				{
					strTemp.Append(delimeter);
					strTemp.Append(wrapper);
					UnpackCurrentTime(currentDatenTime,emptyBuffer);
					strTemp.Append(emptyBuffer);
					strTemp.Append(wrapper);
				}

				if(m_checkCurrentDate.GetCheck() == 1)
				{
					strTemp.Append(delimeter);
					strTemp.Append(wrapper);
					UnpackCurrentDate(currentDatenTime,emptyBuffer);
					strTemp.Append(emptyBuffer);
					strTemp.Append(wrapper);
				}
			}
		}
		//Append the Serial Number
		if (m_checkSerial.GetCheck() == TRUE)
		{
			strTemp.Append(delimeter);
			strTemp.Append(wrapper);
			strTemp.Append( gszFormattedDeviceID );
			strTemp.Append(wrapper);
		}
	} else
	{
		// binairy mode 
		// Not supported on OPN-2001
	}
}

void COPN2001_DEMODlg::ManipulateTimeStamp(CString& tmpRTC, CString& hour, CString& minute, CString& second, CString& ampm, CString& day, CString& month, CString& year)
{
	hour = tmpRTC.Mid( 0, 2);
	if(hour.GetAt(0) == ' ')
	{
		hour.SetAt(0, '0');
	}

	minute = tmpRTC.Mid( 3, 2);
	second = tmpRTC.Mid( 6, 2);
	ampm = tmpRTC.Mid( 9, 2);
	month = tmpRTC.Mid( 12, 2);
	if(month.GetAt(0) == ' ')
	{
		month.SetAt(0, '0');
	}

	day = tmpRTC.Mid( 15, 2);
	year = tmpRTC.Mid( 18, 2);
}

int COPN2001_DEMODlg::HandleDeviceChangeCallback( long nComPort )
{
	//InitOpnListCtrl();
	
	/*if(csp2InitEx(nComPort) != STATUS_OK)
	{
		return -1;
	}*/
	return 0L;
}

static int PollCallBack_sem = 0;

int COPN2001_DEMODlg::IsHandlingPollCallback(void)
{
	return PollCallBack_sem;
}


void __stdcall COPN2001_DEMODlg::FirmwareProgressCallback(short Comport, short Percentage, short Status, LPCTSTR StatusMsg)
{
	COPN2001_DEMODlg *pDlg = (COPN2001_DEMODlg*) pDlgObject;
	return pDlg->HandleFirmwareProgressCallback(Comport, Percentage, Status, StatusMsg);
}

void COPN2001_DEMODlg::HandleFirmwareProgressCallback(short nComPort, short Percentage, short nStatus, LPCTSTR StatusMsg)
{
	CString text;
	CProgressCtrl *pProgressCtrl;
	CStatic *pLabel, *pLabelFile;
	WCHAR wcTempString[ 78 + 2 + 1 ];
	CWnd *hEdit;
	CButton *pBtnBrowse;
	SOpn200xStatus* dev;
	
	if( (dev=GetDeviceStatus(nComPort)) == NULL)
	{
		dev = AddDevice(nComPort);
	}

	wsprintf( wcTempString, L"%S\r\n", StatusMsg );

	pProgressCtrl = (CProgressCtrl*) GetDlgItem( IDC_PROGRESS1 );
	pProgressCtrl->SetPos(Percentage);

	pLabel = (CStatic *)GetDlgItem( IDC_STATIC_PROGRESS_TEXT );
	pLabelFile = (CStatic *)GetDlgItem( IDC_STATIC_FIRMWARE_FILE_TEXT );
	hEdit = GetDlgItem( IDC_FIRMWARE_FILE_EDIT );
	
	pBtnBrowse = (CButton *)GetDlgItem( IDC_BTN_BROWSE2 );

	if (nStatus>=10)
    {
		if (nStatus<1000 || nStatus == DOWNLOAD_FINISHED_COMPLETE)	// Failure or success -> back to normal view
		{
			pLabel->ShowWindow(SW_HIDE);
			pProgressCtrl->ShowWindow(SW_HIDE);
			pLabelFile->ShowWindow(SW_SHOW);
			hEdit->ShowWindow(SW_SHOW);
			pBtnBrowse->ShowWindow(SW_SHOW);
		}
		else if( nStatus == STATUS_CHECKING_FILE)					// Starting update -> show progress bar
		{
			pLabel->ShowWindow(SW_SHOW);
			pProgressCtrl->ShowWindow(SW_SHOW);
			pLabelFile->ShowWindow(SW_HIDE);
			hEdit->ShowWindow(SW_HIDE);
			pBtnBrowse->ShowWindow(SW_HIDE);
			
		}

        if (nStatus<1000)		// Update failed !!!!
		{
			dev->iConnected = FIRMWARE_UPDATE_FAILED;
			UpdateDeviceStatus(dev, false);

			MessageBox(wcTempString,L"Error",MB_OK|MB_ICONERROR);
			
			if( m_btnAuto.GetCheck() == FALSE )
				m_listBcdData.ResetContent();
			else
				pLabel->SetWindowText(L"");

			if(nStatus != ERROR_LOADDLL_NOT_FOUND && nStatus != ERROR_LOADDLL_OUTDATED)
			{
				MessageBox(L"Do NOT disconnect an OPN200x after a failed firmware update. Try restoring the firmware using the 'Firmware Update' button" ,L"Error",MB_OK|MB_ICONERROR);
			}
		}
		else if( nStatus == STATUS_CHECKING_FILE)	// Starting update
		{
			dev->iConnected = FIRMWARE_UPDATE;
			UpdateDeviceStatus(dev, false);

			m_listBcdData.ResetContent();
			m_listBcdData.AddString(wcTempString);
		}
		else if( nStatus == DOWNLOAD_FINISHED_COMPLETE )	// Update completed
		{
			CString strTemp, strDeviceId, strVersion;
			char szDeviceID[ 9 + 1 ];
			char szSwVersion[ 9 + 1 ];

			csp2GetDeviceIdEx( szDeviceID, 9, nComPort); 

			strDeviceId.Format( _T("%02X%02X%02X"), szDeviceID[ 5 ] & 0xFF, 
											szDeviceID[ 6 ] & 0xFF,
											szDeviceID[ 7 ] & 0xFF);

			gszFormattedDeviceID = strDeviceId;
			m_statID.SetWindowText( strDeviceId );

			csp2GetSwVersionEx( szSwVersion, 9, nComPort );
			strVersion.Format(_T("%S"), szSwVersion );
			m_statOSVersion.SetWindowText( strVersion );

			dev->iConnected = TRUE;
			dev->sDeviceId = strDeviceId;
			dev->sSwVersion = strVersion;
			UpdateDeviceStatus(dev, false);

			if( m_btnAuto.GetCheck() == FALSE )
			{
				m_listBcdData.ResetContent();
				MessageBox(L"Download successfully completed" ,L"Info",MB_OK);
			}
		}
		else if (nStatus != STATUS_LOADING && nStatus != STATUS_INIT_IRDA_ADAPTER)	// Ignore these messages
        {
			if( m_btnAuto.GetCheck() == FALSE )
				m_listBcdData.AddString(wcTempString);
			else
				pLabel->SetWindowText(wcTempString);
		}
		else
		{
			if (nStatus == STATUS_INIT_IRDA_ADAPTER)
				text.Format(L"Initializing connection...");
			else
				text.Format(L"%i %% ready",Percentage);

			if( m_btnAuto.GetCheck() )
				pLabel->SetWindowText(text);
			
			m_listBcdData.ResetContent();
			m_listBcdData.AddString(text);
		}
    }
}

int COPN2001_DEMODlg::HandlePollCallback( long nComPort )
{
	char szBarcode[ 2500 + 1 ];
	char szDeviceID[ 9 + 1 ];
	char szSwVersion[ 9 + 1 ];
	char szFirmwareFile[500];
	SYSTEMTIME sysTime;
	long lRet = -1L, lAsciiMode, lRTC;
	int len;
	char curTime[6] = {0};
	char szParam[2] = {0};
	CString strTemp, strDeviceId, strVersion;
	CString strTemp2;
	
	PollCallBack_sem++;

	if(csp2GetDSREx(nComPort))
	{
		if(csp2GetCTSEx(nComPort))
		{
			if( (lRet = csp2ReadDataEx(nComPort)) < 0 )
				TRACE( L"csp2ReadDataEx(%ld) failed!!! %ld \n", nComPort, lRet);
		}
		else
		{
			if( (lRet = csp2InterrogateEx(nComPort)) < 0 )
				TRACE( L"csp2InterrogateEx(%ld) failed!!! %ld \n", nComPort, lRet);
		}

		if( lRet >= 0L && csp2GetTimeEx((unsigned char*)curTime, nComPort) >= 0 )
		{
			// Get packet type (ASCII/Binary)
			lAsciiMode = csp2GetASCIIModeEx(nComPort);

			// Get TimeStamp setting (on/off)
			lRTC = csp2GetRTCModeEx(nComPort);

			csp2GetDeviceIdEx( szDeviceID, 9, nComPort); 

			strDeviceId.Format( _T("%02X%02X%02X"), szDeviceID[ 5 ] & 0xFF, 
											szDeviceID[ 6 ] & 0xFF,
											szDeviceID[ 7 ] & 0xFF);

			gszFormattedDeviceID = strDeviceId;
			m_statID.SetWindowText( strDeviceId );

			csp2GetSwVersionEx( szSwVersion, 9, nComPort );
			strVersion.Format(_T("%S"), szSwVersion );
			m_statOSVersion.SetWindowText( strVersion );
			

			SOpn200xStatus* dev = GetDeviceStatus(nComPort);

			if(dev == NULL)
			{
				dev = AddDevice(nComPort);
				AddToOpnListCtrl(dev);
			}

			dev->iConnected = TRUE;
			dev->sDeviceId = strDeviceId;
			dev->sSwVersion = strVersion;
			dev->nReadBarcodes = lRet;
			
			UpdateOpnListCtrl(nComPort);
			
			// lRet contains the number of barcodes in the OPN-2001
			long lTotalBarcodes = lRet;

			m_listBcdData.ResetContent();
			strTemp.Format( L"Read %ld Barcodes", lRet );
			m_listBcdData.AddString( strTemp );
			
			if(lTotalBarcodes > 0)
			{
				HANDLE hFile = INVALID_HANDLE_VALUE;

				CString SaveFile = m_strSaveFile;
				
				SaveFile.Replace(L"#SERIAL#", strDeviceId);

				//if( m_bSaveToFile ) //Behavior change. now always creates a file regardless of save to file check box state
				//{
					hFile = CreateFile( SaveFile,		// path + filename 
							GENERIC_READ | GENERIC_WRITE,	// open for reading and writing
							0,								// do not share 
							NULL,							// no security 
							OPEN_ALWAYS,					// Open the file create new when not existing
							FILE_ATTRIBUTE_NORMAL,			// normal file 
							NULL);							// no attr. template 
				//}
				if( hFile != INVALID_HANDLE_VALUE )
					SetFilePointer( hFile, 0, NULL, FILE_END); // set file pointer to end for appending

				if ( m_checkShowHeader.GetCheck())			//prepend an optional header to the list.
				{ 
					strTemp = L"" ;
					GenerateHeader( strTemp );

					m_listBcdData.AddString( strTemp );
			/*		
					if( hFile != INVALID_HANDLE_VALUE && firstDownload == true) //Uncomment for appending header to file
					{
						DWORD dwWr;
						len = sprintf_s( szBarcode, sizeof( szBarcode ) -1, "%S\r\n", strTemp );
						WriteFile( hFile, szBarcode, len, &dwWr, NULL );
						firstDownload = false;
					}
			*/	}

				for( long lCount = 0L; lCount < lTotalBarcodes; lCount++ )
				{
					lRet = csp2GetPacketEx( szBarcode, lCount, sizeof( szBarcode ), nComPort);
					if( lRet > 0L )
					{
						ConvertBarcodeToString( szBarcode, lRet-1, strTemp, lAsciiMode, lRTC, curTime);
						CString strTemp2 = strTemp;
						strTemp2.Replace(L"\t", L"    ");
						m_listBcdData.AddString( strTemp2 );

						if( hFile != INVALID_HANDLE_VALUE )
						{
							DWORD dwWr;
							len = sprintf_s( szBarcode, sizeof( szBarcode ) -1, "%S\r\n", strTemp );
							WriteFile( hFile, szBarcode, len, &dwWr, NULL );
						}
					}
				}

				if( hFile != INVALID_HANDLE_VALUE )
					CloseHandle( hFile );

				// remove the barcode data from the OPN
				csp2ClearDataEx(nComPort);
			}

			GetLocalTime( &sysTime );

			curTime[0] = (unsigned char) sysTime.wSecond;	// seconds
			curTime[1] = (unsigned char) sysTime.wMinute;	// minutes
			curTime[2] = (unsigned char) sysTime.wHour;		// hours
			curTime[3] = (unsigned char) sysTime.wDay;		// day
			curTime[4] = (unsigned char) sysTime.wMonth;	// Month
			curTime[5] = (unsigned char) (sysTime.wYear % 100 ); // year

			csp2SetTimeEx( (unsigned char*)curTime, nComPort );

			if(m_bMinimized)
			{
				strTemp.Format( L"Read %ld Barcode(s)", lTotalBarcodes );

				wcsncpy_s(this->m_TrayData.szInfo, strTemp, 64 );
				m_TrayData.uFlags |= NIF_INFO;

				switch(strVersion[2])
				{
					case 'F':	// RFF	
						strTemp.Format(L"OPN2002/3 (ID:%s)", strDeviceId);
						break;

					case 'G':	// RFG
						strTemp.Format(L"OPN2002 (ID:%s)", strDeviceId);
						break;

					case 'I':	// RFI
						strTemp.Format(L"OPN2004/5/6 (ID:%s)", strDeviceId);
						break;

					case 'L':	// RFL
						strTemp.Format(L"OPN2005 (ID:%s)", strDeviceId);
						break;

					case 'M':	// RFM
						strTemp.Format(L"PX20 (ID:%s)", strDeviceId);
						break;

					case 'N':	// RFN
						strTemp.Format(L"OPN2006 (ID:%s)", strDeviceId);
						break;

					default: 	// RBB
						strTemp.Format(L"OPN2001 (ID:%s)", strDeviceId);
						break;
				}
				wcsncpy_s(m_TrayData.szInfoTitle, strTemp, 64);
				m_TrayData.uTimeout = 1000;
				m_TrayData.dwInfoFlags = 0;
				BOOL bSuccess = Shell_NotifyIcon (NIM_MODIFY, &m_TrayData);
				// Zero out the balloon text string so that later operations won't redisplay the balloon.
				m_TrayData.szInfo[0] = _T('\0');
				m_TrayData.uFlags &= ~NIF_INFO;
			}

			// Check if we need to update the firmware
			size_t len;
			wcstombs_s(&len, szFirmwareFile, (LPCTSTR)m_strFirmwareFile, sizeof(szFirmwareFile));
			csp2StartFirmwareUpdateEx(szFirmwareFile, (pFirmwareProgressCallback)&FirmwareProgressCallback, TRUE, nComPort);
		}
		else
		{
			SOpn200xStatus* dev = GetDeviceStatus(nComPort);

			if(dev == NULL)
			{
				dev = AddDevice(nComPort);
				AddToOpnListCtrl(dev);
			}

			dev->iConnected = FALSE;
			UpdateOpnListCtrl(nComPort);
		}
	}
	else
	{
		SOpn200xStatus* dev = GetDeviceStatus(nComPort);

		if(dev == NULL)
		{
			dev = AddDevice(nComPort);
			AddToOpnListCtrl(dev);
		}

		dev->iConnected = FALSE;
		UpdateOpnListCtrl(nComPort);
		
		TRACE( L"COM%ld unplugged!!!\n", nComPort);
	}


	PollCallBack_sem--;

	 // Kick the PollingThreadFunc wait on my exit. 
    if( g_hStopWaitPollCallback != NULL) 
		SetEvent( g_hStopWaitPollCallback ); 
	
	return 0L;

}

void COPN2001_DEMODlg::OnBnClickedBtnGetBarcode()
{
	long lRet, lAsciiMode, lRTC;
	char szBarcode[ 2500 + 1 ];
	int len;
	unsigned char szDeviceID[ 9 + 1 ];
	char curTime[6] = {0};

	CString strTemp;

	lRet = csp2ReadData();

	if( lRet > 0L && csp2GetTime((unsigned char*)curTime) >= 0)
	{
		// Get Device ID and store in a global variable for use later.
		csp2GetDeviceId( (char*)szDeviceID, 9);
		strTemp.Format( _T("%02X%02X%02X"), szDeviceID[ 5 ] & 0xFF, 
											szDeviceID[ 6 ] & 0xFF,
											szDeviceID[ 7 ] & 0xFF);
		gszFormattedDeviceID = strTemp;
		strTemp.Empty();

		// Get packet type (ASCII/Binary)
		lAsciiMode = csp2GetASCIIMode();

		// Get TimeStamp setting (on/off)
		lRTC = csp2GetRTCMode();
		
		// lRet contains the number of barcodes in the OPN-2001
		long lTotalBarcodes = lRet;

		m_listBcdData.ResetContent();
		strTemp.Format( L"Read %ld Barcodes", lRet );
		m_listBcdData.AddString( strTemp );

		HANDLE hFile = INVALID_HANDLE_VALUE;
		if( m_bSaveToFile )
		{
			//CString FileName;
			/*CFileDialog FileDlg(FALSE,_T(".txt"),NULL,0,_T("Text Files(*.txt)|*.txt||"));
			if(FileDlg.DoModal() == IDOK)
			{
				m_strSaveFile = FileDlg.GetFileName();
			}*/

			CString SaveFile = m_strSaveFile;
			
			SaveFile.Replace(L"#SERIAL#", gszFormattedDeviceID);

			hFile = CreateFile( SaveFile,		// path + filename 
					GENERIC_READ | GENERIC_WRITE,	// open for reading and writing
					0,								// do not share 
					NULL,							// no security 
					OPEN_ALWAYS,					// Open the file create new when not existing
					FILE_ATTRIBUTE_NORMAL,			// normal file 
					NULL);							// no attr. template 

			if(hFile == INVALID_HANDLE_VALUE )
			{
				ShowErrorMessage(WRITE_FILE_ERROR);
			}
		}
		if( hFile != INVALID_HANDLE_VALUE )
			SetFilePointer( hFile, 0, NULL, FILE_END); // set file pointer to end for appending
		
		if ( m_checkShowHeader.GetCheck() ) { // then prepend an optional header to the listbox.
			strTemp = L"" ;
			GenerateHeader( strTemp );

			m_listBcdData.AddString( strTemp );
	/*
			if( hFile != INVALID_HANDLE_VALUE && firstDownload == true) //uncomment to save header to the file.
			{
				DWORD dwWr;
				len = sprintf_s( szBarcode, sizeof( szBarcode ) -1, "%S\r\n", strTemp );
				WriteFile( hFile, szBarcode, len, &dwWr, NULL );
				firstDownload = false;
			}
	*/
		}
		for( long lCount = 0L; lCount < lTotalBarcodes; lCount++ )
		{
			lRet = csp2GetPacket( szBarcode, lCount, sizeof( szBarcode ));
			if( lRet > 0L )
			{
				ConvertBarcodeToString( szBarcode, lRet-1, strTemp, lAsciiMode, lRTC, curTime);
				CString strTemp2 = strTemp;
				strTemp2.Replace(L"\t", L"    ");
				m_listBcdData.AddString( strTemp2 );
				if( hFile != INVALID_HANDLE_VALUE )
				{
					DWORD dwWr;
					len = sprintf_s( szBarcode, sizeof( szBarcode ) -1, "%S\r\n", strTemp );
					WriteFile( hFile, szBarcode, len, &dwWr, NULL );
				}
			}
		}
		if( hFile != INVALID_HANDLE_VALUE )
			CloseHandle( hFile );
	}
	else if( lRet == 0L && csp2GetTime((unsigned char*)curTime) >= 0 )
	{
		m_listBcdData.ResetContent();
		m_listBcdData.AddString( L"No barcodes available" );
	}
	else
	{
		ShowErrorMessage( lRet );
	}
}
void COPN2001_DEMODlg::OnBnClickedBtnClearBarcode()
{
	long lRet;

	//MessageBox( strMessage, L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST );
	if( MessageBox( L"This option will delete all data from the OPN-2001. Displayed data and data saved to file is not affected. This action is not reversible.\r\n\r\nDo you wish to proceed?", L"Warning", MB_YESNOCANCEL  | MB_ICONWARNING | MB_TOPMOST ) == IDYES )
	{
		lRet = csp2ClearData();
		if( lRet != STATUS_OK )
		{
			ShowErrorMessage( lRet );
		}
	}
}

void COPN2001_DEMODlg::OnBnClickedBtnParameters()
{
	if( m_btnOpen.IsWindowEnabled() )
	{
		// Show some message that there is no connection to the OPN
		return; 
	}
	
	if(m_nUsedPort > 0)
	{
		std::list<SOpn200xStatus*> EmptyList;
		OnBnClickedBtnParameters(m_nUsedPort, EmptyList);
	}
}

void COPN2001_DEMODlg::OnBnClickedBtnParameters(long m_lComOpen, std::list<SOpn200xStatus*> m_lPortList)
{
	long lRet = csp2InterrogateEx(m_lComOpen);

	if(lRet != STATUS_OK)
	{
		ShowErrorMessage( lRet );
		return;
	}
	
	CDlgParameters dlg;
	dlg.ComPort = m_lComOpen;
	dlg.ComPortList = m_lPortList;
	dlg.DoModal();
}

void COPN2001_DEMODlg::OnBnClickedCheckSave()
{

	CButton *pBtn = (CButton*)GetDlgItem( IDC_CHECK_SAVE );
	m_bSaveToFile = pBtn->GetCheck();

	firstDownload = (m_bSaveToFile != 0); //need to prepend the optional header to the file if we're writing to it.

	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );

	// Store the settings in the INI file
	iniOpnDemo.SetPathName( m_strConfigFile );
	m_bSaveToFile = iniOpnDemo.WriteBool( L"GLOBAL", L"SAVEFILE", m_bSaveToFile ); 
}

void COPN2001_DEMODlg::OnClose()
{
	if(IsHandlingPollCallback())
	{
		csp2DisablePolling();
		WaitForSingleObject( g_hStopWaitPollCallback, 5000UL );
	}

	RemoveAllDevices();

	Shell_NotifyIcon(NIM_DELETE,&m_TrayData);
	CDialog::OnClose();
	EndDialog( IDOK );
}

void COPN2001_DEMODlg::OnOK()
{
}

void COPN2001_DEMODlg::OnCancel()
{
}

void COPN2001_DEMODlg::ShowErrorMessage( long lErrorCode )
{
	CString strMessage;
	bool needDisconnect = false;

	switch( lErrorCode )
	{
	case STATUS_OK:
		strMessage = L"Status OK";
		break;
	case COMMUNICATIONS_ERROR:
		strMessage = L"Communication error!";
		needDisconnect = true;
		break;
	case BAD_PARAM:       
		strMessage = L"Bad parameter!";
		break;
	case SETUP_ERROR:
		strMessage = L"Setup error!";
		break;
	case INVALID_COMMAND_NUMBER:
		strMessage = L"Invalid command number!";
		break;
	case COMMAND_LRC_ERROR:
		strMessage = L"Checksum error!";
		break;
	case RECEIVED_CHARACTER_ERROR:
		strMessage = L"Wrong character received!";
		break;
	case GENERAL_ERROR:
		strMessage = L"General error!";
		break;
	case FILE_NOT_FOUND:
		strMessage = L"File not found!";
		break;
	case ACCESS_DENIED:
		strMessage = L"Access denied!";
		break;
	case WRITE_FILE_ERROR:
		strMessage = L"Couldn't open file for writing";
		break;
	default:
		strMessage = L"Unknown error!!!";
		break;
	}
	MessageBox( strMessage, L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST );
	if(needDisconnect)
	{
		OnBnClickedBtnClose();
	}
}

void COPN2001_DEMODlg::OnBnClickedBtnAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void COPN2001_DEMODlg::InitFormatControls()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );

	m_checkSymbology.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"SHOW_SYMBOLOGY", TRUE ) );
	m_checkTime.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"SHOW_TIME", TRUE ) );
	m_checkDate.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"SHOW_DATE", TRUE ) );
	m_checkSerial.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"SHOW_SERIAL", TRUE ) );
	m_checkCurrentTime.SetCheck(iniOpnDemo.ReadInt( L"FORMAT", L"SHOW_CURRENT_TIME", TRUE ) );
	m_checkCurrentDate.SetCheck(iniOpnDemo.ReadInt( L"FORMAT", L"SHOW_CURRENT_DATE", TRUE ) );
	m_checkShowHeader.SetCheck(iniOpnDemo.ReadInt(L"FORMAT", L"SHOW_DATA_HEADER", TRUE) );

	if(m_checkTime.GetCheck() == 0 && m_checkCurrentTime.GetCheck() == 0)
	{
		m_radiot12.EnableWindow( FALSE );
		m_radiot24.EnableWindow( FALSE );
	}
	if(m_checkDate.GetCheck() == 0 && m_checkCurrentDate.GetCheck() == 0)
	{
		m_radiodUSA.EnableWindow( FALSE );
		m_radiodEU.EnableWindow( FALSE );
		m_radiodLong.EnableWindow( FALSE );
	}

	m_radioText.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"FORMAT_IS_TEXT", TRUE ) );
	m_radioCSV.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"FORMAT_IS_CSV", FALSE ) );

	m_radiot12.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"TIME_IS_12", TRUE ) );
	m_radiot24.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"TIME_IS_24", FALSE ) );

	m_radiodUSA.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"DATE_IS_USA", TRUE ) );
	m_radiodEU.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"DATE_IS_EU", FALSE ) );
	m_radiodLong.SetCheck( iniOpnDemo.ReadInt( L"FORMAT", L"DATE_IS_LONG", FALSE ) );
	
	m_cmboDelimeter.AddString(L"Comma");
	m_cmboDelimeter.AddString(L"Semicolon");

	if(m_radioCSV.GetCheck() != TRUE)
	{
		m_cmboDelimeter.AddString(L"Tab");
	}

	m_cmboDelimeter.SetCurSel( iniOpnDemo.ReadInt( L"FORMAT", L"DELIMETER", 0 ) );
	
	m_strSaveFile = szModulePath;
	
	if(m_radioText.GetCheck() == 1)
	{
		m_strSaveFile += L"\\Barcodes.txt";
	} else {
		m_strSaveFile += L"\\Barcodes.csv";
	}

	m_strSaveFile = iniOpnDemo.ReadString( L"GLOBAL", L"SAVE_FILE_PATH", m_strSaveFile );

	m_strFirmwareFile = szFirmwarePath;
	m_strFirmwareFile = iniOpnDemo.ReadString( L"GLOBAL", L"FIRMWARE_FILE_PATH", m_strFirmwareFile );

	CWnd *hEdit = GetDlgItem( IDC_FIRMWARE_FILE_EDIT );
	hEdit->SetWindowTextW(m_strFirmwareFile);

	strCurSavePath = m_strSaveFile;
	strCurSaveFileName = m_strSaveFile;
	
	LPWSTR m_tmp = strCurSavePath.GetBuffer(1024);
	PathRemoveFileSpec( m_tmp );
	strCurSavePath.ReleaseBuffer();

	LPWSTR m_tmp2 = strCurSaveFileName.GetBuffer(1024);
	PathStripPath( m_tmp2 );
	strCurSaveFileName.ReleaseBuffer();
}

void COPN2001_DEMODlg::OnBnClickedCheckSymbology()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"SHOW_SYMBOLOGY", m_checkSymbology.GetCheck() );
}

void COPN2001_DEMODlg::OnBnClickedCheckSerial()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"SHOW_SERIAL", m_checkSerial.GetCheck() );
}

void COPN2001_DEMODlg::OnBnClickedCheckTime()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"SHOW_TIME", m_checkTime.GetCheck() );

	if(m_checkCurrentTime.GetCheck() == 1 || m_checkTime.GetCheck() == 1)
	{
		m_radiot24.EnableWindow( TRUE );
		m_radiot12.EnableWindow( TRUE );
	}
	else
	{
		m_radiot24.EnableWindow( FALSE );
		m_radiot12.EnableWindow( FALSE );
	}
}

void COPN2001_DEMODlg::OnBnClickedCheckDate()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"SHOW_DATE", m_checkDate.GetCheck() );
	
	if(m_checkCurrentDate.GetCheck() == 1 || m_checkDate.GetCheck() == 1)
	{
		m_radiodUSA.EnableWindow( TRUE );
		m_radiodEU.EnableWindow( TRUE );
		m_radiodLong.EnableWindow( TRUE );
	}
	else
	{
		m_radiodUSA.EnableWindow( FALSE );
		m_radiodEU.EnableWindow( FALSE );
		m_radiodLong.EnableWindow( FALSE );
	}
}

void COPN2001_DEMODlg::OnBnClickedRadioFormatText()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"FORMAT_IS_TEXT", m_radioText.GetCheck() );
	iniOpnDemo.WriteInt( L"FORMAT", L"FORMAT_IS_CSV", !m_radioText.GetCheck() );

	m_cmboDelimeter.AddString(L"Tab");
	m_cmboDelimeter.SetCurSel(0);

	LPWSTR m_tmp = m_strSaveFile.GetBuffer(1024);
	PathRenameExtension( m_tmp, L".txt");
	m_strSaveFile.ReleaseBuffer();

	iniOpnDemo.WriteString( L"GLOBAL", L"SAVE_FILE_PATH", m_strSaveFile);
}

void COPN2001_DEMODlg::OnBnClickedRadioFormatCSV()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"FORMAT_IS_CSV", m_radioCSV.GetCheck() );
	iniOpnDemo.WriteInt( L"FORMAT", L"FORMAT_IS_TEXT", !m_radioCSV.GetCheck() );

	m_cmboDelimeter.DeleteString(2);
	m_cmboDelimeter.SetCurSel(0);

	LPWSTR m_tmp = m_strSaveFile.GetBuffer(1024);
	PathRenameExtension( m_tmp, L".csv");
	m_strSaveFile.ReleaseBuffer();

	iniOpnDemo.WriteString( L"GLOBAL", L"SAVE_FILE_PATH", m_strSaveFile);
}

void COPN2001_DEMODlg::OnBnClickedRadioTime12()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"TIME_IS_12", m_radiot12.GetCheck() );
	iniOpnDemo.WriteInt( L"FORMAT", L"TIME_IS_24", !m_radiot12.GetCheck() );
}

void COPN2001_DEMODlg::OnBnClickedRadioTime24()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"TIME_IS_24", m_radiot24.GetCheck() );
	iniOpnDemo.WriteInt( L"FORMAT", L"TIME_IS_12", !m_radiot24.GetCheck() );
}

void COPN2001_DEMODlg::OnBnClickedRadioDateUSA()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"DATE_IS_USA", m_radiodUSA.GetCheck() );
	iniOpnDemo.WriteInt( L"FORMAT", L"DATE_IS_EU", !m_radiodUSA.GetCheck() );
	iniOpnDemo.WriteInt( L"FORMAT", L"DATE_IS_LONG", !m_radiodUSA.GetCheck() );
}

void COPN2001_DEMODlg::OnBnClickedRadioDateEU()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"DATE_IS_EU", m_radiodEU.GetCheck() );
	iniOpnDemo.WriteInt( L"FORMAT", L"DATE_IS_USA", !m_radiodEU.GetCheck() );
	iniOpnDemo.WriteInt( L"FORMAT", L"DATE_IS_LONG", !m_radiodEU.GetCheck() );
}

void COPN2001_DEMODlg::OnBnClickedRadioDateLong()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"DATE_IS_LONG", m_radiodLong.GetCheck() );
	iniOpnDemo.WriteInt( L"FORMAT", L"DATE_IS_USA", !m_radiodLong.GetCheck() );
	iniOpnDemo.WriteInt( L"FORMAT", L"DATE_IS_EU", !m_radiodLong.GetCheck() );
}

void COPN2001_DEMODlg::OnCbnSelChangeComboDelimeter()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"DELIMETER", m_cmboDelimeter.GetCurSel() );
}

void COPN2001_DEMODlg::OnBnClickedBtnBrowse()
{
	INT_PTR lRet = IDCANCEL;
	CString tmp;

	if(m_radioText.GetCheck() == TRUE)
	{
		CFileDialog FileDlg(FALSE,_T(".txt"),m_strSaveFile,0,_T("Text Files(*.txt)|*.txt||"));
		lRet = FileDlg.DoModal();
		tmp = FileDlg.GetPathName();
	}
	else if(m_radioCSV.GetCheck() == TRUE)
	{
		CFileDialog FileDlg(FALSE,_T(".csv"),m_strSaveFile,0,_T("Comma Seperated Values (*.csv)|*.csv||"));
		lRet = FileDlg.DoModal();
		tmp = FileDlg.GetPathName();
	}
	if(lRet == IDOK)
	{
		CIniFile iniOpnDemo;

		if (m_strSaveFile != tmp )//if a new file path is chosen, then...
			firstDownload = true; //need to prepend the optional header to the file.

		m_strSaveFile = tmp;
		
		iniOpnDemo.SetPathName( m_strConfigFile );
		iniOpnDemo.WriteString( L"GLOBAL", L"SAVE_FILE_PATH", m_strSaveFile);
		
		strCurSavePath = m_strSaveFile;
		strCurSaveFileName = m_strSaveFile;
		
		LPWSTR m_tmp = strCurSavePath.GetBuffer(1024);
		PathRemoveFileSpec( m_tmp );
		strCurSavePath.ReleaseBuffer();

		LPWSTR m_tmp2 = strCurSaveFileName.GetBuffer(1024);
		PathStripPath( m_tmp2 );
		strCurSaveFileName.ReleaseBuffer();
	}
}

void COPN2001_DEMODlg::OnBnClickedBtnBrowse2()
{
	INT_PTR lRet = IDCANCEL;
	CString tmp;

	CFileDialog FileDlg(TRUE,_T(".hex"),m_strFirmwareFile,0,_T("Firmware Files(*.hex)|*.hex||"));
	lRet = FileDlg.DoModal();
	tmp = FileDlg.GetPathName();

	if(lRet == IDOK)
	{
		CIniFile iniOpnDemo;

		m_strFirmwareFile = tmp;
		
		iniOpnDemo.SetPathName( m_strConfigFile );
		iniOpnDemo.WriteString( L"GLOBAL", L"FIRMWARE_FILE_PATH", m_strFirmwareFile);

		CWnd *hEdit = GetDlgItem( IDC_FIRMWARE_FILE_EDIT );
		hEdit->SetWindowTextW(m_strFirmwareFile);

		MessageBox(L"Firmware versions will be checked each time a device is connected.\r\rImportant: Do NOT disconnect a device during a firmware update!",L"Message",MB_OK);
	}
}

struct FirmwareParams
{
	long nComport;
	CString szFirmwareFile;
};


// ----------------------------------------------------------------------------
// Callback thread
DWORD WINAPI FirmwareThreadFunc(LPVOID lpParameter) 
{
	struct FirmwareParams *Params = (FirmwareParams *)lpParameter;
	char szFirmwareFile[500];

	// Check if we need to update the firmware
	size_t len;
	wcstombs_s(&len, szFirmwareFile, (LPCTSTR)Params->szFirmwareFile, sizeof(szFirmwareFile));
	csp2StartFirmwareUpdateEx(szFirmwareFile, (pFirmwareProgressCallback)& COPN2001_DEMODlg::FirmwareProgressCallback, FALSE, Params->nComport );

    // Kick the PollingThreadFunc wait on my exit. 
    if( g_hStopWaitFirmwareUpdate != NULL) 
		SetEvent( g_hStopWaitFirmwareUpdate ); 

    return(STATUS_OK);
}

void COPN2001_DEMODlg::OnBnClickedBtnFirmwareUpdate()
{
	if( m_btnOpen.IsWindowEnabled() )
	{
		// Show some message that there is no connection to the OPN
		return; 
	}

	INT_PTR lRet = IDCANCEL;
	CString tmp;

	CFileDialog FileDlg(TRUE,_T(".hex"),m_strFirmwareFile,0,_T("Firmware Files(*.hex)|*.hex||"));
	lRet = FileDlg.DoModal();
	tmp = FileDlg.GetPathName();

	if(lRet == IDOK)
	{
		CIniFile iniOpnDemo;

		m_strFirmwareFile = tmp;
		
		DoFirmwareUpdate();
	}	
}

void COPN2001_DEMODlg::DoFirmwareUpdate()
{
	static struct FirmwareParams Params;

	DWORD dwThreadID;
    HANDLE hFirmwareThread;

	if(m_nUsedPort > 0)
	{
		if(csp2GetCTS())		// If has data inside
		{
			if( MessageBox( L"Firmware updating might delete all data from the OPN-2001. This action is not reversible.\r\n\r\nDo you wish to proceed?", L"Warning", MB_YESNOCANCEL  | MB_ICONWARNING | MB_TOPMOST ) != IDYES )
				return;
		}

		Params.nComport = m_nUsedPort;
		Params.szFirmwareFile = m_strFirmwareFile;

		// Spawn Thread to invoke callback
		hFirmwareThread = CreateThread( 
			NULL,                        // no security attributes 
			0,                           // use default stack size  
			FirmwareThreadFunc,          // thread function 
			(LPVOID)&Params,		     // argument to thread function 
			0,                           // use default creation flags 
			&dwThreadID);            // returns the thread identifier  

		if( hFirmwareThread != NULL ) 
		{
			// Wait for thread to exit.                 
			WaitForSingleObjectEx( g_hStopWaitFirmwareUpdate, INFINITE, FALSE); 
		}
		else 
		{
			WaitForSingleObject( g_hStopWaitFirmwareUpdate, 50UL );
		}
	}
}

void COPN2001_DEMODlg::OnBnClickedShowAll()
{
	CString strPort;
	CButton *pBtn = (CButton*)GetDlgItem( IDC_SHOW_ALL );
	CIniFile iniOpnDemo;
	int x;

	m_bShowAll = pBtn->GetCheck();

	iniOpnDemo.SetPathName( m_strConfigFile );

	// Fill the combobox
	m_cmboPort.ResetContent(); // Clear the list in the combobox;
	
	RemoveAllDevices();
	
	CSerialEnum::GetSerialPorts( m_nPortArray );

	if(!m_bShowAll)
	{
		//
		// Remove incompatible devices
		//
		m_itPorts = m_nPortArray.begin();

		while (m_itPorts != m_nPortArray.end())
		{
			if( csp2IsOpnCompatiblePort(*m_itPorts) == false && !m_bShowAll )
				m_itPorts = m_nPortArray.erase( m_itPorts );
			else
				m_itPorts++;
		}
	}
					
	for( m_itPorts = m_nPortArray.begin(), x = 0; m_itPorts != m_nPortArray.end(); m_itPorts++, x++ )
	{
		strPort.Format(_T("COM%d"), *m_itPorts );
		m_cmboPort.AddString( strPort );
		m_cmboPort.SetItemData( x, *m_itPorts );
		AddDevice(*m_itPorts);
	}
	
	if( m_nUsedPort == 0 )
		m_cmboPort.SetCurSel( 0 );
	else
	{
		strPort.Format( L"COM%d", m_nUsedPort );
		x = m_cmboPort.FindString( 0, strPort );
		if(x != -1)
			m_cmboPort.SetCurSel( x );
		else
			m_cmboPort.SetCurSel( 0 );
	}

	UpdateOpnListCtrl();
	
	// Store the settings in the INI file
	iniOpnDemo.SetPathName( m_strConfigFile );
	m_bShowAll = iniOpnDemo.WriteBool( L"GLOBAL", L"SHOWALL", m_bShowAll ); 
}

bool startup = true;

void COPN2001_DEMODlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);

	if(m_lComOpen >= 0)
	{
		CStatic* dsrItem = (CStatic*)GetDlgItem(IDC_DSR_STATUS);
		int new_dsr = csp2GetDSR();
		
		if( dsr != new_dsr)
		{
			dsr = new_dsr;
			dsrItem->SetBitmap(::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE( (dsr) ? IDB_CONNECTED : IDB_ERROR)));
		}

		CStatic* ctsItem = (CStatic*)GetDlgItem(IDC_DATA_STATUS);
		int new_cts = csp2GetCTS();
			
		if( cts != new_cts)
		{
			cts = new_cts;
			ctsItem->SetBitmap(::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE( (cts) ? IDB_CONNECTED : IDB_DISCONNECTED)));
		}
	}

	if(startup) // Execute start as tray icon here, because the window wasn't fully created before
	{
		startup=false;

		if(m_checkTrayIcon.GetCheck() && m_btnAuto.GetCheck())
			ShowWindow(SW_MINIMIZE ); 
	}

}
//the following two methods enable current date and time stamping.
void COPN2001_DEMODlg::OnBnClickCheckCTime()
{

	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"SHOW_CURRENT_TIME", m_checkCurrentTime.GetCheck() );
	if(m_checkCurrentTime.GetCheck() == 1 || m_checkTime.GetCheck() == 1)
	{
		m_radiot24.EnableWindow( TRUE );
		m_radiot12.EnableWindow( TRUE );
	}
	else
	{
		m_radiot24.EnableWindow( FALSE );
		m_radiot12.EnableWindow( FALSE );
	}
}

void COPN2001_DEMODlg::OnBnClickedCurrentDate()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"SHOW_CURRENT_DATE", m_checkCurrentDate.GetCheck() );
	
	if(m_checkCurrentDate.GetCheck() == 1 || m_checkDate.GetCheck() == 1)
	{
		m_radiodUSA.EnableWindow( TRUE );
		m_radiodEU.EnableWindow( TRUE );
		m_radiodLong.EnableWindow( TRUE );
	}
	else
	{
		m_radiodUSA.EnableWindow( FALSE );
		m_radiodEU.EnableWindow( FALSE );
		m_radiodLong.EnableWindow( FALSE );
	}
}

void COPN2001_DEMODlg::OnBnClickedShowHeader()
{
	firstDownload = true;
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"FORMAT", L"SHOW_DATA_HEADER", m_checkShowHeader.GetCheck() );
}


RECT COPN2001_DEMODlg::WindowRectangle(void)
{
	if(WindowRect.right == 0)
		this->GetWindowRect(&WindowRect);

	return WindowRect;
}


afx_msg LRESULT COPN2001_DEMODlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
    UINT uID; 
    UINT uMsg; 
 
    uID = (UINT) wParam;
    uMsg = (UINT) lParam; 
   
    if (uID != 1)
      return -1;
  
    CPoint pt;  

    switch (uMsg ) 
    { 
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case 0x405:					// press on icon with message showing
			GetCursorPos(&pt);
			ClientToScreen(&pt);
			MaximizeFromTray();
			break;
		
		case WM_CONTEXTMENU:
			break;
	} 
    return 0; 
}

void COPN2001_DEMODlg::SetupMinimizeToTray()
{
	m_bMinimized = false;

    m_TrayData.cbSize = sizeof(NOTIFYICONDATA);
    m_TrayData.hWnd  = this->m_hWnd; 
    m_TrayData.uID = 1;
    m_TrayData.uCallbackMessage  = WM_TRAY_MESSAGE;
    m_TrayData.hIcon = this->m_hIcon;

	wcscpy_s(m_TrayData.szTip, L"Extended Application for the OPN200x");

	m_TrayData.uTimeout = 100;

    m_TrayData.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;

	m_bMinimized = false;
}

void COPN2001_DEMODlg::MinimizeToTray()
{
	if(m_bMinimized || m_TrayData.uID != 1)
		return;

	m_bMinimized = true;

    this->ShowWindow(SW_HIDE);

	wcscpy_s(this->m_TrayData.szInfo, L"Continues in the background" );
    m_TrayData.uFlags |= NIF_INFO;
    wcscpy_s(m_TrayData.szInfoTitle, L"OPN200x Application");
    m_TrayData.uTimeout = 500;
	m_TrayData.dwInfoFlags = 0;
	
	BOOL bSuccess = Shell_NotifyIcon(NIM_ADD, &m_TrayData);
    if (!(bSuccess))
    {
        MessageBox(L"Unable to set tray icon", L"Error");
    }
}

void COPN2001_DEMODlg::MaximizeFromTray()
{
	if(!m_bMinimized || m_TrayData.uID != 1)
		return;

	m_bMinimized = false;

    this->ShowWindow(SW_SHOW);
	Sleep(100);			// Creates a nicer animation
	this->ShowWindow(SW_SHOWNORMAL);
 
    Shell_NotifyIcon(NIM_DELETE, &m_TrayData);
}

void COPN2001_DEMODlg::OnSize(UINT nType, int cx, int cy) 
{
    if (nType == SIZE_MINIMIZED)
    {
		if(m_checkTrayIcon.GetCheck())
			MinimizeToTray();
    }
    else
    {
        CDialog::OnSize(nType, cx, cy);
    }
}
void COPN2001_DEMODlg::OnBnClickedTrayIcon()
{
	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteInt( L"GLOBAL", L"TRAY_ICON", m_checkTrayIcon.GetCheck() );
}

void COPN2001_DEMODlg::OnBnClickedBtnViewDetails()
{
	m_bDetails = !m_bDetails;
	
	if( m_btnAuto.GetCheck() == 1 )
	{
		if(m_bDetails == FALSE)
			this->SetWindowPos(NULL, 0, 0,  WindowRectangle().right / 2,  WindowRectangle().bottom, SWP_NOZORDER|SWP_NOMOVE);
		else
			this->SetWindowPos(NULL, 0, 0,  WindowRectangle().right,  WindowRectangle().bottom, SWP_NOZORDER|SWP_NOMOVE);
	}

	m_btnDetails.SetWindowText( (m_bDetails) ? L"<<" : L">>" );

	CIniFile iniOpnDemo;
	iniOpnDemo.SetPathName( m_strConfigFile );
	iniOpnDemo.WriteBool( L"GLOBAL", L"DETAILS", m_bDetails); 
}

void COPN2001_DEMODlg::OnNMDblclkListOpns(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	m_listOPNs.ScreenToClient(&pt);
	UINT Flags;
	
	int hItem=m_listOPNs.HitTest( pt, &Flags );

	if(m_DeviceArray.size() == 0)
		return;

	if (Flags & LVHT_ONITEMLABEL) //or whatever
	{
		std::list<SOpn200xStatus *>::iterator m_itDev = m_DeviceArray.begin();
		std::advance(m_itDev, hItem);

		if(IsHandlingPollCallback())	// Ignore uncheck during poll callback to avoid huge crash
		{
			WaitForSingleObject( g_hStopWaitPollCallback, 5000UL );
		}

		csp2DisablePolling();

		SOpn200xStatus *dev = *m_itDev;
		
		OnBnClickedBtnParameters(dev->nComPort, m_DeviceArray);

		csp2EnablePolling();
	}
	
	*pResult = 0; 
}

void COPN2001_DEMODlg::OnLvnItemchangedListOpns(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void COPN2001_DEMODlg::OnEnChangeFirmwareFileEdit()
{
	CString tmp;

	CWnd *hEdit = GetDlgItem( IDC_FIRMWARE_FILE_EDIT );
	hEdit->GetWindowTextW(tmp);

	if (m_strFirmwareFile != tmp )//if a new file path is chosen, then...
	{
		CIniFile iniOpnDemo;

		m_strFirmwareFile = tmp;
		
		iniOpnDemo.SetPathName( m_strConfigFile );
		iniOpnDemo.WriteString( L"GLOBAL", L"FIRMWARE_FILE_PATH", m_strFirmwareFile);
	}
}

void COPN2001_DEMODlg::OnNMRClickListOpns(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	m_listOPNs.ScreenToClient(&pt);
	UINT Flags;
	
	int hItem=m_listOPNs.HitTest( pt, &Flags );

	if(m_DeviceArray.size() == 0)
		return;

	if (Flags & LVHT_ONITEMLABEL) //or whatever
	{
		std::list<SOpn200xStatus*>::iterator m_itDev = m_DeviceArray.begin();
		std::advance(m_itDev, hItem);

		if(IsHandlingPollCallback())	// Ignore uncheck during poll callback to avoid huge crash
		{
			WaitForSingleObject( g_hStopWaitPollCallback, 5000UL );
		}

		csp2DisablePolling();

		SOpn200xStatus *dev = *m_itDev;
		
		long lRet = csp2InterrogateEx(dev->nComPort);

		if(lRet == STATUS_OK)
		{
			csp2PowerDownEx(dev->nComPort);
			csp2InterrogateEx(dev->nComPort);		// forces a beep
		}

		csp2EnablePolling();
	}
	
	*pResult = 0; 
}

BOOL COPN2001_DEMODlg::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	if(pMsg->message == WM_KEYDOWN   )  
	{
		if(pMsg->wParam == _T('D') && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			if(m_nUsedPort > 0 && m_btnAuto.GetCheck() == FALSE)
			{
				OnBnClickedBtnFirmwareUpdate();
			}
		}
		else if(pMsg->wParam == _T('A') && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			DoFirmwareUpdate();
		}
	}
	return CDialog::PreTranslateMessage(pMsg);  
}