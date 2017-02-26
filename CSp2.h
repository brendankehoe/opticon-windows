//-------------------------------------------------------------------------------------------------
// CSP2.H
//
// Terminal: OPN2001 
// 
// Description: API interface for the communication protocol used in the OPN-2001
//

#ifndef __CSP2_H__
#define __CSP2_H__

// use this to make this file also work with normal c-compilers
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CSP2_EXPORTS
	#define CSP2_API __declspec(dllexport) __stdcall
#else
	#define CSP2_API __declspec(dllimport) __stdcall
#endif

//
// Returned status values...
//
#define STATUS_OK                   (0L)
#define COMMUNICATIONS_ERROR        (-1L)  
#define BAD_PARAM                   (-2L)
#define SETUP_ERROR                 (-3L)
#define INVALID_COMMAND_NUMBER      (-4L)  
#define COMMAND_LRC_ERROR           (-7L)  
#define RECEIVED_CHARACTER_ERROR    (-8L)  
#define GENERAL_ERROR               (-9L)  
#define UNINITIALIZED               (-10L)  
#define VERSION_ERROR				(-11L)
#define FIRMWARE_FAILURE            (-12L)  
#define FILE_NOT_FOUND              (2L)
#define ACCESS_DENIED               (5L)

#define DATA_AVAILABLE              (1L)
#define NO_DATA_AVAILABLE           (0L)
#define DETERMINE_SIZE              (0L)

#define PARAM_GS1_DATABAR				20				// See options for PARAM_GS1_DATABAR below
#define PARAM_CODE39					31				// 0: Disable / 1: Enable
#define PARAM_UPC						9				// 0: Disable / 1: Enable
#define PARAM_CODE128					8				// 0: Disable / 1: Enable
#define PARAM_CODE39_FULL_ASCII			54				// 0: Disable / 1: Enable
#define PARAM_UPC_SUPPS					53				// See options for PARAM_UPC_SUPPS below
#define PARAM_UPCE_TO_UPCA				41				// 0: Disable / 1: Enable
#define PARAM_EAN8_TO_EAN13				42				// 0: Disable / 1: Enable
#define PARAM_EAN8_TO_EAN13_TYPE		55				// 0: Disable / 1: Enable
#define PARAM_TX_UPCA_CHECK_DIGIT		43				// 0: Disable / 1: Enable
#define PARAM_TX_UPCE_CHECK_DIGIT		44				// 0: Disable / 1: Enable
#define PARAM_CODE39_CHECK_DIGIT		46				// 0: Disable / 1: Enable
#define PARAM_TX_CODE39_CHECK_DIGIT		45				// 0: Disable / 1: Enable
#define PARAM_UPCE_PREAMBLE				37				// See options for PARAM_UPCE_PREAMBLE below
#define PARAM_UPCA_PREAMBLE				36				// See options for PARAM_UPCA_PREAMBLE below
#define PARAM_EAN128					52				// 0: Disable / 1: Enable
#define PARAM_COUPON_CODE				56				// 0: Disable / 1: Enable
#define PARAM_I2OF5						58				// 0: Disable / 1: Enable
#define PARAM_I2OF5_CHECK_DIGIT			65				// See options for PARAM_I2OF5_CHECK_DIGIT below
#define PARAM_TX_I2OF5_CHECK_DIGIT		64				// 0: Disable / 1: Enable
#define PARAM_ITF14_TO_EAN13			63				// 0: Disable / 1: Enable
#define PARAM_I2OF5_LENGTH_1			59				// Min-max: 0-30
#define PARAM_I2OF5_LENGTH_2			60				// Min-max: 0-30
#define PARAM_D2OF5						57				// 0: Disable / 1: Enable
#define PARAM_D2OF5_LENGTH_1			61				// Min-max: 0-30
#define PARAM_D2OF5_LENGTH_2			62				// Min-max: 0-30
#define PARAM_UPC_EAN_SECURITY			47				// See options for PARAM_UPC_EAN_SECURITY below
#define PARAM_UPC_EAN_REDUNDANCY		48				// See options for PARAM_UPC_EAN_REDUNDANCY below
#define PARAM_SCANNER_ON_TIME			17				// Min-max: 10-100 (1 to 10 seconds)
#define PARAM_BUZZER_VOLUME				2				// See options for PARAM_BUZZER_VOLUME below
#define PARAM_COMM_AWAKE_TIME			32				// Min-max: 1-6
#define PARAM_BAUDRATE					13				// See options for PARAM_BAUDRATE below
#define PARAM_BAUDRATE_SWITCH_DELAY		29				// Min-max: 0-100
#define PARAM_RESET_BAUDRATES			28				// 0: Disable / 1: Enable
#define PARAM_REJECT_REDUNDANT_CODES	4				// See options for PARAM_REJECT_REDUNDANT_CODES below
#define PARAM_HOST_CONNECT_BEEP			10				// 0: Disable / 1: Enable
#define PARAM_HOST_COMPLETE_BEEP		11				// 0: Disable / 1: Enable
#define PARAM_BATTERY_LOW_INDICATION	7				// See options for PARAM_BATTERY_LOW_INDICATION below
#define PARAM_AUTO_CLEAR				15				// 0: Disable / 1: Enable
#define PARAM_DELETE_ENABLE				33				// See options for PARAM_DELETE_ENABLE below
#define PARAM_DATA_PROTECTION			49				// 0: Disable / 1: Enable
#define PARAM_MEMORY_FULL_INDICATION	50				// 0: Disable / 1: Enable
#define PARAM_MEMORY_INDICATION			51				// See options for PARAM_MEMORY_INDICATION below
#define PARAM_MAX_BARCODE_LENGTH		34				// Max barcode length changed from 30 to 254 (255=unlimited) for PX20 (2D symbologies)
#define PARAM_GOODREAD_LED_ONTIME		30				// See options for PARAM_GOODREAD_LED_ONTIME below
#define PARAM_STORE_RTC					35				// 0: Disable / 1: Enable
#define PARAM_ASCII_MODE				79				// 0: Disable / 1: Enable
#define PARAM_BEEPER_TOGGLE				85				// 0: Disable / 1: Enable
#define PARAM_BEEPER_AUTO_ON			86				// 0: Disable / 1: Enable
#define PARAM_LASER_MODE				5				// See options for PARAM_LASER_MODE below
#define PARAM_AIMING_ON_TIME			19				// See options for PARAM_AIMING_ON_TIME below
#define PARAM_SCRATCH_PAD				38				// 32 byte scratch pad

// General Enable / disable parameter options
#define PARAM_DISABLE					0
#define PARAM_ENABLE					1

// General Off / On paramter options
#define PARAM_OFF						0
#define PARAM_ON						1

// Options for PARAM_UPC_SUPPS
#define UPC_NO_SUPPLEMENTS				0
#define UPC_SUPPLEMENTS_ONLY			1
#define UPC_SUPPLEMENTS_AUTO			2

// Options for PARAM_UPCE_PREAMBLE / PARAM_UPCA_PREAMBLE
#define UPC_PREAMBLE_NONE				0
#define UPC_PREAMBLE_SYSTEM				1
#define UPC_PREAMBLE_SYSTEM_N_COUNTRY	2

// Options for PARAM_I2OF5_CHECK_DIGIT
#define I2OF5_CHECK_DIGIT_DISABLED		0
#define I2OF5_CHECK_DIGIT_USS			1
#define I2OF5_CHECK_DIGIT_OPCC			2

// Options for PARAM_BAUDRATE
#define BAUDRATE_300					3
#define BAUDRATE_600					4
#define BAUDRATE_1200					5
#define BAUDRATE_2400					6
#define BAUDRATE_4800					7
#define BAUDRATE_9600					8
#define BAUDRATE_19200					9

// Options for PARAM_BATTERY_LOW_INDICATION (bit flags)
#define BATTLOW_NO_OPERATION_NO_INDICATION		0
#define BATTLOW_INDICATION_ENABLED				1
#define BATTLOW_ALLOW_OPERATION					2

// Options for PARAM_DELETE_ENABLE (bit flags)
#define CLEAR_ALL_ENABLED						1			// Allow clearing of all data by pressing delete key for 6 seconds
#define DELETE_ENABLED							2			// Allow deleting of a barcode from memory using the delete key
#define SET_DEFAULT_ENABLED						4			// Allow reset to factory default by pressing boths keys for 10 seconds

// Options for PARAM_GOODREAD_LED_ONTIME
#define GOODREAD_ONTIME_250MS					1
#define GOODREAD_ONTIME_500MS					2
#define GOODREAD_ONTIME_750MS					3
#define GOODREAD_ONTIME_1000MS					4

// Options for PARAM_GS1_DATABAR (bit flags)
#define GS1_DATABAR_DISABLED					0
#define GS1_DATABAR_LIMITED						1
#define GS1_DATABAR_14							2
#define GS1_DATABAR_EXPANDED					4

// Options for PARAM_LASER_MODE
#define LASER_MODE_NORMAL						0
#define LASER_MODE_AIMING_HELP					1
#define LASER_MODE_SMALL						2

// Options for PARAM_REJECT_REDUNDANT_CODES
#define REJECT_REDUNDANT_CODES_OFF				0
#define REJECT_CONSECUTIVE_REDUNDANT_CODES		1
#define REJECT_ANY_REDUNDANT_CODES				2

// Options for PARAM_BUZZER_VOLUME
#define BUZZER_DISABLED							0
#define BUZZER_MAXIMUM							1
#define BUZZER_LOUD								2
#define BUZZER_NORMAL							3
#define BUZZER_SOFT								4
#define BUZZER_MINIMUM							5

// Options for PARAM_MEMORY_INDICATION
#define MEMORY_INDICATION_DISABLED				0
#define MEMORY_INDICATION_90PCT					1
#define MEMORY_INDICATION_NOT_EMPTY				2

//
// Defines for firmware update messages
//
#define ERROR_CONNECTION_LOST		70 
#define ERROR_LOADDLL_NOT_FOUND		998
#define ERROR_LOADDLL_OUTDATED		999
#define DOWNLOAD_FINISHED_COMPLETE	1020
#define STATUS_ERASING_MEMORY		2010
#define STATUS_LOADING				2020
#define STATUS_INIT_IRDA_ADAPTER	2030
#define STATUS_CHECKING_FILE		2040
#define STATUS_FINDING_ROM_OS_VERSION	2050
#define STATUS_ERASE_APPLICATION	2060
#define STATUS_ERASE_OS				2070
#define STATUS_SET_TIME				2080
#define STATUS_RECEIVING_DATA_FILE	2090

//
// Strange way of selecting serial ports
// Should only be used with the csp2Init() function
// 
typedef enum __serial_ports{
	COM1 = 0,
	COM2,
	COM3,
	COM4,
	COM5,
	COM6,
	COM7,
	COM8,
	COM9,
	COM10,
	COM11,
	COM12,
	COM13,
	COM14,
	COM15,
	COM16
}_serial_ports;

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2Init( long nCOMPort );
//
// Description:		Open the COM port with one of the _serial_ports enum
//					The values that can be used are from COM1 .. COM16
//					Do not use normal numbers
//
// Argument:	nCOMPort the constant _serial_ports number from COM1 .. COM16
//
// Important:		COM1 is used when nComport == 0 see _serial_ports enum,
//					COM16 is the highest port number that can be used, when higher ports are 
//					needed use the csp2InitEx() function.
//
// Returns:			0L		on success 
//					< 0L	on failure 
//
long CSP2_API csp2Init( long nCOMPort );	// important COM1 is initializes when nCOMPort = 0 supports up to COM16

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2InitEx( long nCOMPort );
//
// Description:	Open the COM port. This functions supports from COM1 .. COM255.
//				Do not use the constants from the _serial_ports enum, but use 
//				1L for COM1, 2 for COM2 etc..
//
// Argument:	nCOMPort serial port number
//
// Important:	COM1 is used when nCOMPort == 1
//
// Returns:		0L		on success 
//				< 0L	on failure 
//
long CSP2_API csp2InitEx( long nCOMPort );	// important COM1 is initializes when nComport = 1 supports all available COM ports

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2Restore( void );
//
// Description:	Closes the COM port that was opened by the csp2Init or csp2InitEx
//
// Returns:		0L		on success 
//				< 0L	on failure 
//
long CSP2_API csp2Restore( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2RestoreEx( long nCOMPort );
//
// Description:	Closes the COM port that is specified
//
// Returns:		0L		on success 
//				< 0L	on failure 
//
long CSP2_API csp2RestoreEx( long nCOMPort );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2Wakeup( void );
//
// Description:	Set the DTR (data-terminal-ready) signal. This function is added 
//				for compatibility reasons.
//
// Returns:		0L		on success 
//				< 0L	on failure 
//
long CSP2_API csp2WakeUp( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2WakeupEx( long nCOMport );
//
// Description:	Set the DTR (data-terminal-ready) signal of the specified com port. This function is added 
//				for compatibility reasons.
//
// Returns:		0L		on success 
//				< 0L	on failure 
//
long CSP2_API csp2WakeUpEx( long nCOMPort );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2DataAvailable( void );
//
// Description:	Checks if CTS (clear-to-send) signal is ON. When the CTS line is ON
//				Data is available in the OPN-2001
//
// Returns:		> 0L		Data available
//				0L			No data available
//				< 0L		on failure 
//
long CSP2_API csp2DataAvailable( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2DataAvailableEx( long nCOMPort );
//
// Description:	Checks if CTS (clear-to-send) signal is ON. When the CTS line is ON
//				Data is available in the OPN-2001 of the specified com port
//
// Returns:		> 0L		Data available
//				0L			No data available
//				< 0L		on failure 
//
long CSP2_API csp2DataAvailableEx( long nCOMPort );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2ReadData( void );
//
// Description:	Read all data from the OPN-2001 and store it in an 
//				internal buffer. To read data from the internal buffer use 
//				one of the functions below.
//					* csp2GetPacket()
//					* csp2GetDeviceId()
//
// Returns:		>= 0L		The amount of barcodes that are collected
//				< 0L		on failure 
long CSP2_API csp2ReadData( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2ReadDataEx( long nCOMPort );
//
// Description:	Reads all data from the OPN-2001 on the specified com port and stores it in an 
//				internal buffer. To read data from the internal buffer use 
//				one of the functions below.
//					* csp2GetPacket()
//					* csp2GetDeviceId()
//
// Returns:		>= 0L		The amount of barcodes that are collected
//				< 0L		on failure 
long CSP2_API csp2ReadDataEx( long nCOMPort );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2ClearData( void );
//
// Description:	Removes all collected barcodes from the OPN-2001
//
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2ClearData( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2ClearDataEx( long nCOMPort );
//
// Description:	Removes all collected barcodes from the OPN-2001 on the specified comport
//
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2ClearDataEx( long nCOMPort );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2PowerDown( void );
//
// Description:	Send a power down command to the OPN-2001.
//				This function was added for compatibility reasons only.
//				
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2PowerDown( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2PowerDownEx( long nCOMPort );
//
// Description:	Send a power down command to the OPN-2001 on the specified com port.
//				This function was added for compatibility reasons only.
//				
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2PowerDownEx( long nCOMPort );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetTime( unsigned char ucTime[] );
//
// Description:	Get the current time from the OPN-2001.
//
// Argument:	ucTime needs to be character buffer of at least 6 bytes
//				ucTime[ 0 ] = Seconds
//				ucTime[ 1 ] = Minutes
//				ucTime[ 2 ] = Hours
//				ucTime[ 3 ] = Day
//				ucTime[ 4 ] = Month
//				ucTime[ 5 ] = Year
//				
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2GetTime( unsigned char ucTime[] );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetTimeEx( unsigned char ucTime[], long nCOMport );
//
// Description:	Get the current time from the OPN-2001 on the specified com port.
//
// Argument:	ucTime	See 'csp2GetTime'
//				
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2GetTimeEx( unsigned char ucTime[], long nCOMPort );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetTime( unsigned char ucTime[] );
//
// Description:	Set a new time and date in the OPN-2001
//
// Argument:	ucTime needs to be a character buffer of at least 6 bytes
//				ucTime[ 0 ] = Seconds
//				ucTime[ 1 ] = Minutes
//				ucTime[ 2 ] = Hours
//				ucTime[ 3 ] = Day
//				ucTime[ 4 ] = Month
//				ucTime[ 5 ] = Year
//				
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2SetTime( unsigned char ucTime[] );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetTimeEx( unsigned char ucTime[], long nCOMport );
//
// Description:	Set a new time and date in the OPN-2001 on the specified com port
//
// Argument:	ucTime	See 'csp2SetTime'
//				
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2SetTimeEx( unsigned char ucTime[], long nCOMport );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetDefault( void );
//
// Description:	Set the OPN-2001 into default mode
//				
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2SetDefaults( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetDefaultEx( long nCOMport );
//
// Description:	Set the OPN-2001 on the specified com port into default mode
//				
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2SetDefaultsEx( long nCOMport );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetPacket( char szData[], long nBcrNr, long nMaxDataSz );
//
// Description:	Get data that was stored in the internal buffer by the csp2ReadData function.
//
// Argument:	szData[] buffer where the barcode data is stored in
//				nBcrNr the wanted barcode number starting from 0L
//				nMaxDataSz the maximum size of the szData buffer
//				
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2GetPacket( char szData[], long nBcrNr, long nMaxDataSz );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetPacketEx( char szData[], long nBcrNr, long nMaxDataSz, long nCOMport );
//
// Description:	Get data that was stored in the internal buffer by the csp2ReadData function
//				from the OPN-2001 on the specified com port.
//
// Argument:	szData[] buffer where the barcode data is stored in
//				nBcrNr the wanted barcode number starting from 0L
//				nMaxDataSz the maximum size of the szData buffer
//				nCOMport specifies the com port of the OPN-2001 from which the data is requested
//				
// Returns:		0L		on success
//				< 0L	on failure 
//
long CSP2_API csp2GetPacketEx( char szData[], long nBcrNr, long nMaxDataSz, long nCOMport );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetDeviceId( char cDeviceId[8], long nMaxDeviceIdSz );
//
// Description:	Get the device ID number from the OPN-2001
//
// Parameters:	cDeviceId[8] is the buffer where the device ID is copied into
//				nMaxDeviceIdSz is the maximum size of the szDeviceId buffer
//			
// Important:	In the cDeviceId buffer an Hex respresentation of the Device ID is placed
//
// Returns:		0L	0n success
//				< 0L		on failure 
//
long CSP2_API csp2GetDeviceId( char cDeviceId[8], long nMaxDeviceIdSz );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetDeviceIdEx( char cDeviceId[8], long nMaxDeviceIdSz, long nCOMport );
//
// Description:	Get the device ID number from the OPN-2001 on the specified com port
//
// Parameters:	cDeviceId[8] is the buffer where the device ID is copied into
//				nMaxDeviceIdSz is the maximum size of the szDeviceId buffer
//				nCOMport is the com port from the OPN-2001 from which the device ID is read
//			
// Important:	In the cDeviceId buffer an Hex respresentation of the Device ID is placed
//
// Returns:		0L	0n success
//				< 0L		on failure 
//
long CSP2_API csp2GetDeviceIdEx( char cDeviceId[8], long nMaxDeviceIdSz, long nCOMport );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetProtocol( void );
//
// Description:	Get the OPN-2001 protocol number. 
//				
// Returns:		>= 0L	Protocol version number
//				< 0L	on failure 
//
long CSP2_API csp2GetProtocol( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetProtocolEx( long nCOMport );
//
// Description:	Get the protocol number of the OPN-2001 on the specified com port. 
//				
// Returns:		>= 0L	Protocol version number
//				< 0L	on failure 
//
long CSP2_API csp2GetProtocolEx( long nCOMport );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetSystemStatus( void );
//
// Description:	Get the systemstatus of the OPN-2001.
//				
// Returns:		= STATUS_OK		System OK
//				= 22L			Low battery
//				< 0L			on failure 
//
long CSP2_API csp2GetSystemStatus( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetSystemStatusEx( long nCOMport );
//
// Description:	Get the systemstatus of the OPN-2001 on the specified com port.
//				
// Returns:		= STATUS_OK		System OK
//				= 22L			Low battery
//				< 0L			on failure 
//
long CSP2_API csp2GetSystemStatusEx( long nCOMport );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetSwVersion( char szSwVersion[9], long nMaxSwVersionSz );
//
// Description:	Get the software version string from OPN-2001
//
// Argument:	szSwVersion[9] buffer where the software version is copied into
//				nMaxLength the maximum size of the szSwVersion
//				
// Returns:		>= 0L		The needed buffer size to collect all version data including the '\0' character
//				< 0L		on failure 
//
long CSP2_API csp2GetSwVersion( char szSwVersion[9], long nMaxSwVersionSz );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetSwVersionEx( char szSwVersion[9], long nMaxSwVersionSz, long nCOMport );
//
// Description:	Get the software version string from OPN-2001 on the specified com port
//
// Argument:	szSwVersion[9] buffer where the software version is copied into
//				nMaxLength the maximum size of the szSwVersion
//				
// Returns:		>= 0L		The needed buffer size to collect all version data including the '\0' character
//				< 0L		on failure 
//
long CSP2_API csp2GetSwVersionEx( char szSwVersion[9], long nMaxSwVersionSz, long nCOMport );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetASCIIMode( void );
//
// Description:	Returns a value telling if the OPN-2001 is in ASCII mode
//				
// Returns:		= 1L		ASCII mode ON
//				= 0L		ASCII mode OFF
//				< 0L		on failure 
//
long CSP2_API csp2GetASCIIMode( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetASCIIModeEx( long nCOMport );
//
// Description:	Returns a value telling if the OPN-2001 on the specified com port is in ASCII mode
//				
// Returns:		= 1L		ASCII mode ON
//				= 0L		ASCII mode OFF
//				< 0L		on failure 
//
long CSP2_API csp2GetASCIIModeEx( long nCOMport );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetRTCMode( void );
//
// Description:	Return value tells if the OPN-2001 adds a timestamp to the 
//				collected barcode data.
//				
// Returns:		= 1L		Timstamp append to barcode data
//				= 0L		No timestamp is used
//				< 0L		on failure 
//
long CSP2_API csp2GetRTCMode( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetRTCModeEx( long nCOMport );
//
// Description:	Return value tells if the OPN-2001 on the specified com port adds  
//				a timestamp to the collected barcode data.
//				
// Returns:		= 1L		Timstamp append to barcode data
//				= 0L		No timestamp is used
//				< 0L		on failure 
//
long CSP2_API csp2GetRTCModeEx( long nCOMport );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetRetryCount( long nRetries );
//
// Description:	Set the number of interrogation retries 
//
// Argument:	nRetries specifies the number interrogation retries
//				values from 1L .. 9L are supported
//				
// Returns:		STATUS_OK	on success
//				< 0L		on failure 
//
long CSP2_API csp2SetRetryCount( long nRetries );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetRetryCount( void );
//
// Description:	Get the number of communication retries 
//
// Returns:		1 .. 9 the number of retries
long CSP2_API csp2GetRetryCount( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetDefaultTimeout( long nTimeout );
//
// Description:	Set the communication timeout in ms 
//
// Argument:	nTimeout specifies the communication timeout in ms
//				values allowed from 100...9999 (default = 1000)
//				
// Returns:		STATUS_OK	on success
//				< 0L		on failure 
//
long CSP2_API csp2SetDefaultTimeout( long nTimeout );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetDefaultTimeout( void );
//
// Description:	Get the communication timeout in ms 
//
// Returns:		communication timeout in ms
long CSP2_API csp2GetDefaultTimeout( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetDllVersion( char szDllVersion[], long nMaxDllVersionSz );
//
// Description:	Get the software version of the DLL
//
// Argument:	szDllVersion[] buffer where the software version is copied into
//				nMaxDllVersionSz the maximum size of the szDllVersion
//				
// Returns:		STATUS_OK		on success
//				FILE_NOT_FOUND	on failure 
//
long CSP2_API csp2GetDllVersion( char szDllVersion[], long nMaxDllVersionSz );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2TimeStamp2Str( unsigned char *ucTimeStamp, char *szTimeStampResult, long nMaxTimeStampResult );
//
// Description:	convert the packed ucTimeStamp array to a normal 'timestamp' string this implementation has some 
//				problems but is needed for compatibility reasons
//
// Argument:	ucTimeStamp pointer to the packed 4 byte timestamp
//				szTimeStampResult string representation of the packed ucTimeStamp array
//				nMaxTimeStampResult the maximum size of the szTimeStampResult buffer
//
// Important:	The szTimeStampResult buffer needs to be at leat 21 bytes big.
//				time format: HH:MM:SS AM MM/DD/YYYY  
//				
// Returns:		STATUS_OK	on success
//				< 0L		on failure 
//
long CSP2_API csp2TimeStamp2Str( unsigned char *ucTimeStamp, char *szTimeStampResult, long nMaxTimeStampResult );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2TimeStamp2StrEx( unsigned char *ucTimeStamp, char *szTimeStampResult, long nMaxTimeStampResult );
//
// Description:	convert the packed ucTimeStamp array to a normal 'timestamp' string this is the correct implementation
//
// Argument:	ucTimeStamp pointer to the packed 4 byte timestamp
//				szTimeStampResult string representation of the packed ucTimeStamp array
//				nMaxTimeStampResult the maximum size of the szTimeStampResult buffer
//
// Important:	The szTimeStampResult buffer needs to be at leat 21 bytes big.
//				time format: HH:MM:SS AM MM/DD/YYYY  
//				
// Returns:		STATUS_OK	on success
//				< 0L		on failure 
//
long CSP2_API csp2TimeStamp2StrEx( unsigned char *ucTimeStamp, char *szTimeStampResult, long nMaxTimeStampResult );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetCodeType( unsigned long nBcrCodeID, char *szCodeTypeBuf, long nMaxCodeTypeBuf );
//
// Description:	convert the OPN-2001 barcode code id to a readable format
//
// Argument:	nBcrCodeID integer value of the code ID
//				szCodeTypeBuf pointer to the result buffer
//				nMaxCodeTypeBuf the maximum size of the szCodeTypeBuf buffer
//				
// Returns:		STATUS_OK	on success
//				< 0L		on failure 
//
long CSP2_API csp2GetCodeType( unsigned long nBcrCodeID, char *szCodeTypeBuf, long nMaxCodeTypeBuf );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2ReadRawData( char aResultBuf[], long nMaxResultBuf );
//
// Description:	Read data from the OPN-2001 and store it in aResultBuf until 
//				nMaxResultBuf bytes are read.
//				When nMaxResultBuf == 0L the csp2ReadRawData will return the 
//				size of the result buffer needed to collect all the data from 
//				the OPN-2001. 
//
// Argument:	aResultBuf result buffer where the readed data is copied into.
//				nMaxResultBuf the maximum size of the aResultBuf
//
// Returns:		>= 0L		The needed buffer size to collect all data
//				< 0L		on failure 
//
long CSP2_API csp2ReadRawData( char aResultBuf[], long nMaxResultBuf );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2ReadRawDataEx( char aResultBuf[], long nMaxResultBuf, long nCOMport );
//
// Description:	Read data from the OPN-2001 and store it in aResultBuf until 
//				nMaxResultBuf bytes are read.
//				When nMaxResultBuf == 0L the csp2ReadRawData will return the 
//				size of the result buffer needed to collect all the data from 
//				the OPN-2001. 
//
// Argument:	aResultBuf result buffer where the readed data is copied into.
//				nMaxResultBuf the maximum size of the aResultBuf
//				nCOMport specifies the com port on which the OPN-2001 is connected
//
// Returns:		>= 0L		The needed buffer size to collect all data
//				< 0L		on failure 
//
long CSP2_API csp2ReadRawDataEx( char aResultBuf[], long nMaxResultBuf, long nCOMport );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetParam(long nParamNr, char szNewParam[], long nMaxNewParam);
//
// Description:	Write one individual device parameter at one time
//
// Parameters:	nParamNr the parameter number to change
//				szNewParam hold the new parameter setting
//				nMaxNewParam  specifies the amount of characters in szNewParam that 
//				makeup the new parameter settings
//
// Returns:		STATUS_OK	On success
//				< 0L		on failure 
long CSP2_API csp2SetParam(long nParamNr, char szNewParam[], long nMaxNewParam );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetParamEx(long nParamNr, char szNewParam[], long nMaxNewParam, long nCOMport);
//
// Description:	Write one individual device parameter at one time
//
// Parameters:	nParamNr the parameter number to change
//				szNewParam hold the new parameter setting
//				nMaxNewParam  specifies the amount of characters in szNewParam that 
//				makeup the new parameter settings
//				nCOMport specifies the com port on which the OPN-2001 is connected
//
// Returns:		STATUS_OK	On success
//				< 0L		on failure 
long CSP2_API csp2SetParamEx(long nParamNr, char szNewParam[], long nMaxNewParam, long nCOMport );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetParam(long nParamNr, char szResultParam[], long nMaxResultParam);
//
// Description:	Read one individual device parameter at one time
//
// Argument:	nParamNr the parameter number to read
//				szResultParam pointer to the buffer that contains the readed parameter setting
//				nMaxLength specifies maximum size of szResultParam can contain
//
// Returns:		STATUS_OK	On success
//				< 0L		on failure 
//
long CSP2_API csp2GetParam(long nParamNr, char szResultParam[], long nMaxResultParam );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetParamEx(long nParamNr, char szResultParam[], long nMaxResultParam, long nCOMport);
//
// Description:	Read one individual device parameter at one time from the OPN-2001 on the specified com port
//
// Argument:	nParamNr the parameter number to read
//				szResultParam pointer to the buffer that contains the readed parameter setting
//				nMaxLength specifies maximum size of szResultParam can contain
//				nCOMport specifies the com port of the OPN-2001
//
// Returns:		STATUS_OK	On success
//				< 0L		on failure 
//
long CSP2_API csp2GetParamEx(long nParamNr, char szResultParam[], long nMaxResultParam, long nCOMport );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2Interrogate( void );
//
// Description:	Request a response of the OPN-2001, this fill also the internal buffer for
//				software version, protocol version and system status.
//					* csp2GetSwVersion() 
//					* csp2GetProtocol()
//					* csp2GetSystemStatus()
//
// Returns:		STATUS_OK	on success
//				< 0L		on failure
long CSP2_API csp2Interrogate( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2InterrogateEx( long nCOMport );
//
// Description:	Request a response of the OPN-2001 on the specified com port, this fill also 
//				the internal buffer for	software version, protocol version and system status.
//					* csp2GetSwVersion() 
//					* csp2GetProtocol()
//					* csp2GetSystemStatus()
//
// Returns:		STATUS_OK	on success
//				< 0L		on failure
long CSP2_API csp2InterrogateEx( long nCOMport );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetCTS( void );
//
// Description:	returns the CTS (clear-to-send) signal status.
//
// Returns:		1L			ON
//				0L			OFF
//				< 0L		on failure
//
long CSP2_API csp2GetCTS( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetCTSEx( long nCOMport );
//
// Description:	returns the CTS (clear-to-send) signal status of the OPN-2001 on the specified com port.
//
// Returns:		1L			ON
//				0L			OFF
//				< 0L		on failure
//
long CSP2_API csp2GetCTSEx( long nCOMport );


//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetDTR( long nOn );
//
// Description:	Set the DTR (data-terminal-ready) flow control line status
//
// Argument:	nOn = 1L set DTR line ON (high), nOn = 0L clears DTR line to OFF (low)
//
// Returns:		STATUS_OK	on success
//				< 0L		on failure
//
long CSP2_API csp2SetDTR( long nOn );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetDTREx( long nOn, long nCOMport );
//
// Description:	Set the DTR (data-terminal-ready) flow control line status
//				of the OPN-2001 on the specified com port 
//
// Argument:	nOn = 1L set DTR line ON (high), nOn = 0L clears DTR line to OFF (low)
//
// Returns:		STATUS_OK	on success
//				< 0L		on failure
//
long CSP2_API csp2SetDTREx( long nOn, long nCOMport );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetDSR( void );
//
// Description:	returns the DSR (data-signal-ready) signal status.
//
// Returns:		1L			ON
//				0L			OFF
//				< 0L		on failure
//
long CSP2_API csp2GetDSR( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetDSREx( long nCOMport );
//
// Description:	returns the DSR (data-signal-ready) signal status of the OPN-2001 
//				on the specified com port.
//
// Returns:		1L			ON
//				0L			OFF
//				< 0L		on failure
//
long CSP2_API csp2GetDSREx( long nCOMport );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2SetDebugMode( long nOn );
//
// Description:	Sets the debug version of the CSP2.DLL in debug mode.
//
// Argument:	nOnO 1L == ON, 0L == OFF
//
// Important:	Works only with the debug version of the CSP2.DLL
//
// Returns:		STATUS_OK	on success
//				< 0L		on failure
//
long CSP2_API csp2SetDebugMode( long nOn );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2StartPolling( FARPROC csp2CallBack );
//
// Description:	This function creates a new thread to poll for OPN-2001s.
//				The csp2CallBack function address is called by the thread to 
//				notify that a OPN-2001 is detected.
//
// Argument:	csp2CallBack address of the callback function
//				format of callback function int __stdcall csp2PollCallBack( void )
//
// Returns:		STATUS_OK	on success
//				< 0L		on failure
//
long CSP2_API csp2StartPolling( FARPROC csp2CallBack );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2StartPollingAll( int (FAR WINAPI *csp2CallBack)(long) );
//
// Description:	This function creates a new thread to poll for OPN-2001s.
//				The csp2CallBack function address is called by the thread to 
//				notify that a OPN-2001 is detected and on which com port.
//
// Argument:	csp2CallBack address of the callback function
//				format of callback function int __stdcall csp2PollCallBack( long nCOMport )
//
// Returns:		STATUS_OK	on success
//				< 0L		on failure
//
long CSP2_API csp2StartPollingAll( int (FAR WINAPI *csp2CallBack)(long) );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2StopPolling( void );
//
// Description:	Stops the polling thread and remove it from memory
//
// Returns:		STATUS_OK	on success
//				< 0L		on failure
long CSP2_API csp2StopPolling( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2EnablePolling( void );
//
// Description:	Signals the polling thread to resume operation.
//
// Returns:		STATUS_OK	on success
//				< 0L		on failure
//
long CSP2_API csp2EnablePolling( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2DisablePolling( void );
//
// Description:	Signals the polling thread  to into suspend mode 
//
// Returns:		STATUS_OK	on success
//				< 0L		on failure
long CSP2_API csp2DisablePolling( void );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetOpnCompatiblePorts( void );
//
// Description:	Returns a list of OPN2001 compatible COM ports
//
// Returns:		> 1			on success (amount of compatible devices)
//				= 0L		on none found
//				< 0L		on failure
long CSP2_API csp2GetOpnCompatiblePorts( long PortArray[] );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2IsOpnCompatiblePort( int port );
//
// Description:	Returns whether if the specified port is an OPN2001 compatible device
//
// Argument:	nCOMPort serial port number
//
// Returns:		false		port unknown or not OPN2001 compatible
//				true		port is OPN2001 compatible
bool CSP2_API csp2IsOpnCompatiblePort( long nCOMPort );

// Callback function prototype
typedef void (FAR WINAPI *pFirmwareProgressCallback)( short Comport, short Percentage, short Status, LPCTSTR StatusMsg );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2StartFirmwareUpdate(LPCSTR Filename, void (FAR WINAPI *csp2FirmwareUpdateProgress)( short, short, short, LPCTSTR ), bool bCheckVersion );
//
// Description:	This function start a firmware update on the last used COM-port
//				The ProgressCallBack function address is called to notify about progress
//
// Argument:	Filename 	Filename of the firmware file
//				ProgressCallback	Format of callback function void xxxxxx( short Comport, short Percentage, short Status, char *StatusMsg )
//				bCheckVersion		If true, the version of the file is checked with the version of the device. When the file is not compatible or the version
//									is the same, then the firmware update is not started.
//									
//
// Returns:		STATUS_OK	on su ccess
//				< 0L		on failure
//
long CSP2_API csp2StartFirmwareUpdate(LPCSTR Filename, pFirmwareProgressCallback ProgressCallback, bool bCheckVersion );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2StartFirmwareUpdateEx(char *Filename, pFirmwareProgressCallback ProgressCallback, bool bCheckVersion, long nCOMPort );
//
// Description:	This function start a firmware update
//				The csp2FirmwareUpdateProgress CallBack function address is called to notify about progress
//
// Argument:	Filename 			Filename of the firmware file
//				ProgressCallback	Format of callback function void xxxxxx( short Comport, short Percentage, short Status, char *StatusMsg )
//				bCheckVersion		If true, the version of the file is checked with the version of the device. When the file is not compatible or the version
//									is the same, then the firmware update is not started.
//				nCOMPort			serial port number
//
// Returns:		STATUS_OK	on su ccess
//				< 0L		on failure
//
long CSP2_API csp2StartFirmwareUpdateEx(LPCSTR Filename, pFirmwareProgressCallback ProgressCallback, bool bCheckVersion, long nCOMPort );

//-------------------------------------------------------------------------------------------------
// long CSP2_API csp2GetLoadDllVersion( char szDllVersion[], long nMaxDllVersionSz );
//
// Description:	Get the software version of the LoadDLL
//
// Argument:	szDllVersion[] buffer where the software version is copied into
//				nMaxDllVersionSz the maximum size of the szDllVersion
//				
// Returns:		STATUS_OK		on success
//				FILE_NOT_FOUND	on failure 
//
long CSP2_API csp2GetLoadDllVersion( char szDllVersion[], long nMaxDllVersionSz );

//-------------------------------------------------------------------------------------------------
// Obsolete functions 
// 
// The functions below are obsolete and should therefore not be used
// The declaration of these function have been added for compatibility reasons only.
//
long CSP2_API csp2GetCommInfo( long nCOMPort );
long CSP2_API csp2SetMultiParam( char szMultiParams[], long nMaxMultiParams );

#ifdef __cplusplus
}
#endif	// ifdef __cplusplus 


#endif // __CSP2_H__
