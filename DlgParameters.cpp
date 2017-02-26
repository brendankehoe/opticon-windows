// DlgParameters.cpp : implementation file
//

#include "stdafx.h"
#include "OPN2001_DEMO.h"
#include "DlgParameters.h"
#include "csp2.h"
#include ".\GridCtrl\GridCellCombo.h"
#include ".\GridCtrl\GridCellNumeric.h"

long ComPort;
std::list<SOpn200xStatus> ComPortList;

typedef struct 
{
	LPCWSTR szParameter;
	short	nTypeMin;	// if nTypeMin is 9999 then combobox or 9998 as special input (scratch pad)
	short	nTypeMax;	// if nTypeMin is 9999 then nTypeMax selects the combobox type	
	short	nOffset;	// Some comboboxes start with another value then zero the use the nOffset value
	BYTE	nParamNr;
}ParamOptions;

ParamOptions paramOptions [] = 
{
	L"GS1 DataBar",                     9999,   10, 0,  PARAM_GS1_DATABAR,				// See options for PARAM_GS1_DATABAR below
	L"Code-39",							9999,	0,	0,	PARAM_CODE39,					// 0: Disable / 1: Enable
	L"UPC",								9999,	0,	0,	PARAM_UPC,						// 0: Disable / 1: Enable
	L"Code-128",						9999,	0,	0,	PARAM_CODE128,					// 0: Disable / 1: Enable
	L"Code-39 Full ASCII",				9999,	0,	0,	PARAM_CODE39_FULL_ASCII,		// 0: Disable / 1: Enable
	L"UPC Supps",						9999,	2,	0,	PARAM_UPC_SUPPS,				// See options for PARAM_UPC_SUPPS below
	L"Convert UPC-E to A",				9999,	0,	0,	PARAM_UPCE_TO_UPCA,				// 0: Disable / 1: Enable
	L"Convert EAN-8 to EAN-13",			9999,	0,	0,	PARAM_EAN8_TO_EAN13,			// 0: Disable / 1: Enable
	L"Convert EAN-8 to EAN-13 Type",	9999,	0,	0,	PARAM_EAN8_TO_EAN13_TYPE,		// 0: Disable / 1: Enable
	L"Send UPC-A Check Digit",			9999,	0,	0,	PARAM_TX_UPCA_CHECK_DIGIT,		// 0: Disable / 1: Enable
	L"Send UPC-E Check Digit",			9999,	0,	0,	PARAM_TX_UPCE_CHECK_DIGIT,		// 0: Disable / 1: Enable
	L"Code-39 Check Digit",				9999,	0,	0,	PARAM_CODE39_CHECK_DIGIT,		// 0: Disable / 1: Enable
	L"Xmit Code-39 Check Digit",		9999,	0,	0,	PARAM_TX_CODE39_CHECK_DIGIT,	// 0: Disable / 1: Enable
	L"UPC-E Preamble",					9999,	3,	0,	PARAM_UPCE_PREAMBLE,			// See options for PARAM_UPCE_PREAMBLE below
	L"UPC-A Preamble",					9999,	3,	0,	PARAM_UPCA_PREAMBLE,			// See options for PARAM_UPCA_PREAMBLE below
	L"EAN-128",							9999,	0,	0,	PARAM_EAN128,					// 0: Disable / 1: Enable
	L"Coupon Code",						9999,	0,	0,	PARAM_COUPON_CODE,				// 0: Disable / 1: Enable
	L"I 2 of 5",						9999,	0,	0,	PARAM_I2OF5,					// 0: Disable / 1: Enable
	L"I 2of5 Check Digit",				9999,	4,	0,	PARAM_I2OF5_CHECK_DIGIT,		// See options for PARAM_I2OF5_CHECK_DIGIT below
	L"Xmit I 2of5 Check Digit",			9999,	0,	0,	PARAM_TX_I2OF5_CHECK_DIGIT,		// 0: Disable / 1: Enable
	L"Convert ITF14 to EAN 13",			9999,	0,	0,	PARAM_ITF14_TO_EAN13,			// 0: Disable / 1: Enable
	L"I 2of5 Length 1",					0,		30, 0,	PARAM_I2OF5_LENGTH_1,			// Min-max: 0-30
	L"I 2of5 Length 2",					0,		30,	0,	PARAM_I2OF5_LENGTH_2,			// Min-max: 0-30
	L"D 2of5",							9999,	0,	0,	PARAM_D2OF5,					// 0: Disable / 1: Enable
	L"D 2of5 Length 1",					0,		30,	0,	PARAM_D2OF5_LENGTH_1,			// Min-max: 0-30
	L"D 2of5 Length 2",					0,		30,	0,	PARAM_D2OF5_LENGTH_2,			// Min-max: 0-30
	L"UPC/EAN Security",				0,		3,	0,	PARAM_UPC_EAN_SECURITY,			// See options for PARAM_UPC_EAN_SECURITY below
	L"UPC/EAN Supplemental Redundancy", 2,     20,  0,	PARAM_UPC_EAN_REDUNDANCY,		// See options for PARAM_UPC_EAN_REDUNDANCY below
	L"Scanner On-Time",					10,		100,0,	PARAM_SCANNER_ON_TIME,			// Min-max: 10-100 (1 to 10 seconds)
	L"Volume",							9999,	13,	0,	PARAM_BUZZER_VOLUME,			// See options for PARAM_BUZZER_VOLUME below
	L"Comm awake time",					1,		6,	0,	PARAM_COMM_AWAKE_TIME,			// Min-max: 1-6
	L"Baudrate",						9999,	5,	3,	PARAM_BAUDRATE,					// See options for PARAM_BAUDRATE below
	L"Baud Switch Delay",				0,		100,0,	PARAM_BAUDRATE_SWITCH_DELAY,	// Min-max: 0-100
	L"Reset Baudrates",					9999,	0,	0,	PARAM_RESET_BAUDRATES,			// 0: Disable / 1: Enable
	L"Reject redundant barcode",		9999,	12,	0,	PARAM_REJECT_REDUNDANT_CODES,	// See options for PARAM_REJECT_REDUNDANT_CODES below
	L"Host Connect Beep",				9999,	0,	0,	PARAM_HOST_CONNECT_BEEP,		// 0: Disable / 1: Enable
	L"Host Complete Beep",				9999,	0,	0,	PARAM_HOST_COMPLETE_BEEP,		// 0: Disable / 1: Enable
	L"Low-battery indication",			9999,	6,	0,	PARAM_BATTERY_LOW_INDICATION,	// See options for PARAM_BATTERY_LOW_INDICATION below
	L"Auto Clear",						9999,	0,	0,	PARAM_AUTO_CLEAR,				// 0: Disable / 1: Enable
	L"Delete Enable",					9999,	7,	0,	PARAM_DELETE_ENABLE,			// See options for PARAM_DELETE_ENABLE below
	L"Data Protection",					9999,	0,	0,	PARAM_DATA_PROTECTION,			// 0: Disable / 1: Enable
	L"Memory full indication",			9999,	0,	0,	PARAM_MEMORY_FULL_INDICATION,	// 0: Disable / 1: Enable
	L"Memory (Low) Indication",			9999,	14,	0,	PARAM_MEMORY_INDICATION,		// See options for PARAM_MEMORY_INDICATION below
	L"Max Barcode Length (255=unlimited)",1,    255,0,	PARAM_MAX_BARCODE_LENGTH,		// Max barcode length changed from 30 to 254 (255=unlimited) for PX20 (2D symbologies)
	L"Good decode LED on time",			9999,	8,	1,	PARAM_GOODREAD_LED_ONTIME,		// See options for PARAM_GOODREAD_LED_ONTIME below
	L"Store RTC",						9999,	0,	0,	PARAM_STORE_RTC,				// 0: Disable / 1: Enable
	L"ASCII mode",						9999,	9,	0,	PARAM_ASCII_MODE,				// 0: Disable / 1: Enable
	L"Beeper Toggle",					9999,	1,	0,	PARAM_BEEPER_TOGGLE,			// 0: Disable / 1: Enable
	L"Beeper Auto On",					9999,	1,	0,	PARAM_BEEPER_AUTO_ON,			// 0: Disable / 1: Enable
	L"Laser mode",						9999,	11,	0,	PARAM_LASER_MODE,				// See options for PARAM_LASER_MODE below
	L"Aiming On-Time",					10,		100,0,	PARAM_AIMING_ON_TIME,			// See options for PARAM_AIMING_ON_TIME below
//	L"Scratch Pad",						9998,	0,	0,	PARAM_SCRATCH_PAD,				// 32 byte scratch pad (Disabled because PX20 and OPN2005/6 had a bug giving the wrong response)
};

#define MAX_OPN_PARAMETERS ( sizeof(paramOptions) / sizeof(ParamOptions))


// CDlgParameters dialog

IMPLEMENT_DYNAMIC(CDlgParameters, CDialog)

CDlgParameters::CDlgParameters(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgParameters::IDD, pParent)
	, m_pProgWnd( NULL )
{
	// Enable disable string options
	m_paramStringOptions[ 0 ].Add(L"0 - Disable");
	m_paramStringOptions[ 0 ].Add(L"1 - Enable");

	// On off string options
	m_paramStringOptions[ 1 ].Add( L"0 - Off");
	m_paramStringOptions[ 1 ].Add( L"1 - On");

	// UPC Supps string options
	m_paramStringOptions[ 2 ].Add( L"0 - No Supps");
	m_paramStringOptions[ 2 ].Add( L"1 - Supps only");
	m_paramStringOptions[ 2 ].Add( L"2 - Auto-D");

	// UPC-A and UPC-E preamble string options
	m_paramStringOptions[ 3 ].Add( L"0 - None");
	m_paramStringOptions[ 3 ].Add( L"1 - System char");
	m_paramStringOptions[ 3 ].Add( L"2 - Sys char & country code");

	// I 2of5 Check Digit string options
	m_paramStringOptions[ 4 ].Add( L"0 - Disabled");
	m_paramStringOptions[ 4 ].Add( L"1 - USS check digit");
	m_paramStringOptions[ 4 ].Add( L"2 - OPCC check digit");

	// Baudrate string options
	m_paramStringOptions[ 5 ].Add( L"3 - 300");
	m_paramStringOptions[ 5 ].Add( L"4 - 600");
	m_paramStringOptions[ 5 ].Add( L"5 - 1200");
	m_paramStringOptions[ 5 ].Add( L"6 - 2400");
	m_paramStringOptions[ 5 ].Add( L"7 - 4800");
	m_paramStringOptions[ 5 ].Add( L"8 - 9600");
	m_paramStringOptions[ 5 ].Add( L"9 - 19200");

	// Low-Battery Indication string options
	m_paramStringOptions[ 6 ].Add( L"0 - No indication / No operation");
	m_paramStringOptions[ 6 ].Add( L"1 - No Indication / Allow operation");
	m_paramStringOptions[ 6 ].Add( L"2 - Indicate / No operation");
	m_paramStringOptions[ 6 ].Add( L"3 - Indicate / Allow operation.");

	// Delete enable string options
	m_paramStringOptions[ 7 ].Add( L"0 - Delete OFF / Clear all OFF / Set default OFF");
	m_paramStringOptions[ 7 ].Add( L"1 - Delete OFF / Clear all ON / Set default OFF");
	m_paramStringOptions[ 7 ].Add( L"2 - Delete ON / Clear all OFF / Set default OFF");
	m_paramStringOptions[ 7 ].Add( L"3 - Delete ON / Clear all ON / Set default OFF");
	m_paramStringOptions[ 7 ].Add( L"4 - Delete OFF / Clear all OFF / Set default ON");
	m_paramStringOptions[ 7 ].Add( L"5 - Delete OFF / Clear all ON / Set default ON");
	m_paramStringOptions[ 7 ].Add( L"6 - Delete ON / Clear all OFF / Set default ON");
	m_paramStringOptions[ 7 ].Add( L"7 - Delete ON / Clear all ON / Set default ON");

	// Good Decode LED On Time string options
	m_paramStringOptions[ 8 ].Add( L"1 - 250 ms");
	m_paramStringOptions[ 8 ].Add( L"2 - 500 ms");
	m_paramStringOptions[ 8 ].Add( L"3 - 750 ms");
	m_paramStringOptions[ 8 ].Add( L"4 - 1000 ms");

	// ASCII mode string options
	m_paramStringOptions[ 9 ].Add( L"0 - Like encrypted data");
	m_paramStringOptions[ 9 ].Add( L"1 - As ASCII strings");

	m_paramStringOptions[10].Add(L"0 - Disable");
	m_paramStringOptions[10].Add(L"1 - Enable GS1 Databar Limited");
	m_paramStringOptions[10].Add(L"2 - Enable GS1 Databar");	
	m_paramStringOptions[10].Add(L"3 - Enable GS1 Databar and Limited");
	m_paramStringOptions[10].Add(L"4 - Enable GS1 Databar Expanded");
	m_paramStringOptions[10].Add(L"5 - Enable GS1 Databar Limited and Exp.");
	m_paramStringOptions[10].Add(L"6 - Enable GS1 Databar and Expanded");
	m_paramStringOptions[10].Add(L"7 - Enable GS1 Databar all symbologies");

	m_paramStringOptions[11].Add(L"0 - Normal");
	m_paramStringOptions[11].Add(L"1 - Aiming help");
	m_paramStringOptions[11].Add(L"2 - Small laser");	

	// Reject redundant barcodes options
	m_paramStringOptions[12].Add( L"0 - Disabled");
	m_paramStringOptions[12].Add( L"1 - Consecutive");
	m_paramStringOptions[12].Add( L"2 - Any");

	// Buzzer volume options
	m_paramStringOptions[13].Add( L"0 - Off");
	m_paramStringOptions[13].Add( L"1 - On (Maximum)");
	m_paramStringOptions[13].Add( L"2 - On (Loud)");
	m_paramStringOptions[13].Add( L"3 - On (Normal)");
	m_paramStringOptions[13].Add( L"4 - On (Soft)");
	m_paramStringOptions[13].Add( L"5 - On (Minimum)");

	// Indicate on memory low / not empty
	m_paramStringOptions[14].Add( L"0 - Disabled");
	m_paramStringOptions[14].Add( L"1 - 90% full");
	m_paramStringOptions[14].Add( L"2 - Not empty");
}

CDlgParameters::~CDlgParameters()
{
}

void CDlgParameters::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID_PARAMETERS, m_gridParam);
}


BEGIN_MESSAGE_MAP(CDlgParameters, CDialog)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_PARAMETERS, OnGridEndEdit)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_PARAMETERS, OnGridEndSelChange)
	ON_BN_CLICKED(IDC_BTN_DEFAULT, &CDlgParameters::OnBnClickedBtnDefault)
	ON_BN_CLICKED(IDC_BTN_SETTINGS, &CDlgParameters::OnBnClickedBtnSettings)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_RESET, &CDlgParameters::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_APPLY_TO_ALL, &CDlgParameters::OnBnClickedBtnApplyToAll)
END_MESSAGE_MAP()


// CDlgParameters message handlers
BOOL CDlgParameters::OnInitDialog()
{
	CString strTitle;
        
	CDialog::OnInitDialog();

	strTitle.Format(_T("Parameters (COM%d)"), ComPort);

	CDialog::SetWindowTextW(strTitle);

	CRect rect;
	m_gridParam.SetColumnCount( 2 );
	m_gridParam.SetRowCount( MAX_OPN_PARAMETERS );
	m_gridParam.SetFixedColumnCount();
	m_gridParam.GetClientRect( &rect );
	m_gridParam.SetColumnWidth( 0, ((rect.Width() / 3) * 2)-14);
	m_gridParam.SetColumnWidth( 1, (rect.Width() / 3 )+18); 
	m_gridParam.SetFixedColumnSelection( FALSE );
	m_gridParam.SetFixedRowSelection( FALSE );
	m_gridParam.EnableTitleTips( FALSE );

	CButton* pBtn = (CButton*)GetDlgItem( IDC_BTN_APPLY_TO_ALL );

	pBtn->EnableWindow( (ComPortList.size() != 0) );

	// Create a progress window
	if( m_pProgWnd == NULL )
	{
        strTitle.Format(_T("Progress (COM%d)"), ComPort);
		m_pProgWnd = new CProgressWnd( this, strTitle, 0 );
	
		if( m_pProgWnd != NULL )
			m_pProgWnd->Hide();
	}

	// Fill the list control
	FillListControl(); 

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDlgParameters::FillListControl( void )
{
	long lRet;
	int nPercentage;
	BYTE szParam[ 32+1 ];
	CString strItem;

	CString strTitle;

	m_pProgWnd->m_strTitle.Format(_T("Progress (COM%d)"), ComPort);

	m_pProgWnd->Show();
	for( int x = 0; x < MAX_OPN_PARAMETERS; x++ )
	{
		m_gridParam.SetItemText( x, 0, paramOptions[ x ].szParameter );
		m_pProgWnd->SetText( paramOptions[ x ].szParameter );
		nPercentage = ((x + 1)*100) / MAX_OPN_PARAMETERS; 
		m_pProgWnd->SetPos( (int) nPercentage );

		if( (lRet =  csp2GetParamEx( paramOptions[ x ].nParamNr, (char *)szParam, (paramOptions[ x ].nTypeMin != 9998) ? 1L : sizeof(szParam)-1, ComPort )) != STATUS_OK )
			ShowErrorMessage( lRet );
//		if( lRet == STATUS_OK )
//		{
//			strItem.Format(L"%d", szParam[ 0 ]);
//		}

		if( paramOptions[ x ].nTypeMin == 9999 )
		{
			szParam[ 0 ] -= (BYTE)paramOptions[ x ].nOffset;
			// Combobox type 
			m_gridParam.SetCellType( x, 1, RUNTIME_CLASS(CGridCellCombo));

			if( szParam[ 0 ] < 0 || szParam[ 0 ] >= m_paramStringOptions[ paramOptions[ x ].nTypeMax ].GetCount())
				szParam[ 0 ] = 0;

			m_gridParam.SetItemText( x, 1, m_paramStringOptions[ paramOptions[ x ].nTypeMax ].ElementAt( szParam[ 0 ] ));
			CGridCellCombo *pCell = (CGridCellCombo*) m_gridParam.GetCell( x, 1 );
			pCell->SetOptions( m_paramStringOptions[ paramOptions[ x ].nTypeMax ] );
			pCell->SetStyle( CBS_DROPDOWNLIST );
			
		}
		else if( paramOptions[ x ].nTypeMin == 9998 )
		{
			// Special option
			szParam[32] = '\0';
			strItem.Format( L"%S", szParam );
			m_gridParam.SetItemText( x, 1, strItem );
		}
		else
		{
			// Min max options
			m_gridParam.SetCellType( x, 1, RUNTIME_CLASS( CGridCellNumeric ));
			if( szParam[ 0 ] < (BYTE)paramOptions[ x ].nTypeMin || szParam[ 0 ] > (BYTE)paramOptions[ x ].nTypeMax )
				szParam[ 0 ] = (BYTE)paramOptions[ x ].nTypeMin;

			strItem.Format( L"%d", (int)szParam[ 0 ] );
			m_gridParam.SetItemText( x, 1, strItem );
		}
	}
	m_gridParam.UpdateData();
	m_gridParam.Invalidate();
	m_pProgWnd->Hide();
}


// GVN_ENDLABELEDIT
void CDlgParameters::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int nValue;
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	CGridCellNumeric *pCell = (CGridCellNumeric*)m_gridParam.GetCell( pItem->iRow, pItem->iColumn );
	nValue = _ttoi( pCell->GetText());

	if( paramOptions[ pItem->iRow ].nTypeMin == 9999 || paramOptions[ pItem->iRow ].nTypeMin == 9998 )
	{
		*pResult = 0;
	}
	else
	{
		if( nValue < paramOptions[ pItem->iRow ].nTypeMin || nValue > paramOptions[ pItem->iRow ].nTypeMax )
			*pResult = -1; // Wrong value
		else
			*pResult = 0; // OK
	}
}

// GVN_SELCHANGED
void CDlgParameters::OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( pItem->iColumn == 1 && paramOptions[ pItem->iRow ].nTypeMin == 9998 )
	{
		if( paramOptions[ pItem->iRow ].nTypeMax == 0 )
		{
			// scratch pad option
			//::MessageBox( NULL, L"Scratch pad", L"Information", MB_OK);
			return;
		}
	}
}

void CDlgParameters::OnBnClickedBtnDefault()
{
	long lRet;

	lRet = csp2SetDefaultsEx(ComPort);
	if( lRet != STATUS_OK )
	{
		ShowErrorMessage( lRet );
		return;
	}
	FillListControl();
}

void CDlgParameters::OnBnClickedBtnSettings(void)
{
	OnBnClickedBtnSettings(ComPort);
}

void CDlgParameters::OnBnClickedBtnSettings(long nComPort)
{
	long lRet;
	int nPercentage;
	char szParam[ 2 ];
	char szScratchPad[ 32+1 ];
	CString strItem;

	m_pProgWnd->m_strTitle.Format(_T("Progress (COM%d)"), nComPort);
	m_pProgWnd->Show();
	for( int x = 0; x < MAX_OPN_PARAMETERS; x++ )
	{
		m_pProgWnd->SetText( paramOptions[ x ].szParameter );
		nPercentage = ((x + 1)*100) / MAX_OPN_PARAMETERS; 
		m_pProgWnd->SetPos( (int) nPercentage );

		if( paramOptions[ x ].nTypeMin == 9999 )
		{
			szParam[ 0 ] = 0;
			// combobox type
			for( int itemnr = 0; itemnr < m_paramStringOptions[ paramOptions[ x ].nTypeMax ].GetCount(); itemnr++ )
			{
				CString strData = m_paramStringOptions[ paramOptions[ x ].nTypeMax ].ElementAt( itemnr ) ;
				if( m_paramStringOptions[ paramOptions[ x ].nTypeMax ].ElementAt( itemnr ).Compare( m_gridParam.GetCell( x, 1 )->GetText() ) == 0)
				{
					szParam[ 0 ] = itemnr + paramOptions[ x ].nOffset;
					break;
				}
			}
		}
		else if( paramOptions[ x ].nTypeMin == 9998 )
		{
			// special options, e.g. scratch pad
			wcstombs( szScratchPad, m_gridParam.GetCell( x, 1)->GetText(), sizeof(szScratchPad)-1 );

			if( (lRet = csp2SetParamEx( paramOptions[ x ].nParamNr, szScratchPad, (long)strnlen(szScratchPad, sizeof(szScratchPad)-1), nComPort )) != STATUS_OK )
				ShowErrorMessage( lRet );
			
			continue;
		}
		else
		{
			szParam[ 0 ] = _ttoi( m_gridParam.GetCell( x, 1)->GetText());
		}
		if( (lRet = csp2SetParamEx( paramOptions[ x ].nParamNr, szParam, 1L, nComPort )) != STATUS_OK )
			ShowErrorMessage( lRet );
	}

	m_pProgWnd->Hide();
}


void CDlgParameters::OnClose()
{
	if( m_pProgWnd != NULL )
		delete m_pProgWnd;
	m_pProgWnd = NULL;
	CDialog::OnClose();
	EndDialog( IDOK );
}

void CDlgParameters::OnOK()
{
}

void CDlgParameters::OnCancel()
{
}


void CDlgParameters::ShowErrorMessage( long lErrorCode )
{
	CString strMessage;

	switch( lErrorCode )
	{
	case STATUS_OK:
		strMessage = L"Status OK";
		break;
	case COMMUNICATIONS_ERROR:
		strMessage = L"Communication error!";
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
		strMessage = L"Unknow error!!!";
		break;
	}
	MessageBox( strMessage, L"Error", MB_OK | MB_ICONERROR );
}


void CDlgParameters::OnBnClickedBtnReset()
{
	FillListControl();
}

void CDlgParameters::OnBnClickedBtnApplyToAll()
{
	if(ComPortList.size() == 0)
		return;

	std::list<SOpn200xStatus*>::iterator m_itDev;
	
	for(m_itDev = ComPortList.begin(); m_itDev != ComPortList.end(); m_itDev++)
	{
		SOpn200xStatus *dev = *m_itDev;

		if(dev->iConnected == TRUE)
			OnBnClickedBtnSettings(dev->nComPort);
	}
}
