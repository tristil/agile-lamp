// GenericHIDDlg.cpp : implementation file
//

#include "stdafx.h"
#include <dbt.h>
#include "GenericHID.h"
#include "GenericHIDDlg.h"
#include "USBHid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenericHIDDlg dialog

CGenericHIDDlg::CGenericHIDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGenericHIDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGenericHIDDlg)
	m_nOutputPattern = 0;
	m_strInputDisplay = _T("");
	m_nReadLength = 16;
	m_nWriteLength = 16;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGenericHIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenericHIDDlg)
	DDX_Radio(pDX, IDC_OUTPUTPATTERN1, m_nOutputPattern);
	DDX_Text(pDX, IDC_INPUTDISPLAY, m_strInputDisplay);
	//}}AFX_DATA_MAP
//	DDX_Text(pDX, IDC_OUTLENGTH, m_nWriteLength);
//	DDV_MinMaxInt(pDX, m_nWriteLength, 0, 65535);
}

BEGIN_MESSAGE_MAP(CGenericHIDDlg, CDialog)
	//{{AFX_MSG_MAP(CGenericHIDDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_INPUT, OnInput)
	ON_BN_CLICKED(IDC_OUTPUT, OnOutput)
	ON_MESSAGE(WM_DISPLAY_READ_DATA, OnDisplayReadData)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_DEVICECHANGE,OnDeviceChange)
	ON_MESSAGE(WM_UNREGISTER_HANDLE,OnUnregisterHandle)
	ON_MESSAGE(WM_DEVICE_REMOVE_DONE,OnDeviceRemoveDone)
	ON_MESSAGE(WM_DEVICE_FIND_DONE,OnDeviceFindDone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenericHIDDlg message handlers

BOOL CGenericHIDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// Register the notification for HID devices.
	DEV_BROADCAST_DEVICEINTERFACE    broadcastInterface;

    broadcastInterface.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    broadcastInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    HidD_GetHidGuid(&broadcastInterface.dbcc_classguid);

    m_hHidNotificationHandle = RegisterDeviceNotification(m_hWnd,
                                                &broadcastInterface,
                                                DEVICE_NOTIFY_WINDOW_HANDLE);

	if(m_USBHidDevice.FindDevice(m_hWnd)) {
		m_bFoundDevice = TRUE;
	} else {
		MessageBox("Cannot find the generic HID device. Please plug in the device.", "Error", MB_ICONWARNING | MB_OK);
		if(m_USBHidDevice.FindDevice(m_hWnd)) {	//Find again.
			m_bFoundDevice = TRUE;
		} else {
			UpdateUI(FALSE);
			m_bFoundDevice = FALSE;
		}
	}

	SetDlgItemInt(IDC_LENGTH, m_nReadLength);
	//	keybd_event(0xaa, 0, 0, 0); 
	//	keybd_event(0xaa, 0, KEYEVENTF_KEYUP, 0); 

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGenericHIDDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGenericHIDDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGenericHIDDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGenericHIDDlg::OnInput() 
{
	int nActualLength;
	CString strByteData;
	m_strInputDisplay.Empty();

	m_nReadLength = GetDlgItemInt(IDC_LENGTH);
	if(m_nReadLength>255 || m_nReadLength<1) {
		MessageBox("Input length is only valid between 1 and 255.", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	PUCHAR pszByteData = (PUCHAR)calloc(sizeof(UCHAR), m_nReadLength);

	if(m_USBHidDevice.ReadBuffer(pszByteData, m_nReadLength, &nActualLength)) {

		for(WORD wCount=0; wCount<m_nReadLength; wCount++) {
			if(wCount%8 == 7) {
				strByteData.Format("%2x\r\n", pszByteData[wCount]);
			} else {
				strByteData.Format("%2x ", pszByteData[wCount]);
			}
			m_strInputDisplay += strByteData;
		}
		UpdateData(FALSE);
		MessageBox("Data has been read and displayed.", "GenericHID", MB_OK | MB_ICONINFORMATION);
	} else {
		MessageBox("Read device failed", "Error", MB_OK | MB_ICONERROR);
	}
	delete(pszByteData);
}


void CGenericHIDDlg::OnOutput() 
{
	int nActualLength;
	PCHAR szByteData;
	UCHAR ucCount;

	UpdateData(TRUE);
	m_nWriteLength = 16; // Now only length 16 is available.
	szByteData = (PCHAR)calloc(sizeof(char), m_nWriteLength);


	switch(m_nOutputPattern) {
	case 0:
		for(ucCount=0; ucCount<m_nWriteLength; ucCount++) {
			szByteData[ucCount] = (CHAR)0x5a;
		}
		break;
	case 1:
		for(ucCount=0; ucCount<m_nWriteLength; ucCount++) {
			szByteData[ucCount] = (CHAR)(ucCount*ucCount);
		}
		break;
	case 2:
		for(ucCount=0; ucCount<m_nWriteLength; ucCount++) {
			szByteData[ucCount] = ucCount;
		}	
		break;
	case 3:
		for(ucCount=0; ucCount<m_nWriteLength; ucCount++) {
			szByteData[ucCount] = 0xff - ucCount;
		}	
		break;
	}

	CString strDisplay, strByteData;
	strDisplay = "Data { ";
	for(ucCount=0; ucCount<m_nWriteLength; ucCount++) {
		if((ucCount%16==0) && ucCount) {
			strByteData.Format("\r\n%2x ", (UCHAR)szByteData[ucCount]);
		} else {
			strByteData.Format("%2x ", (UCHAR)szByteData[ucCount]);
		}
		strDisplay+=strByteData;
	}	
	strDisplay += " } is going to be written into device.";
	if(MessageBox(strDisplay, "GenericHID", MB_OKCANCEL | MB_ICONINFORMATION)==IDOK) {
		if(m_USBHidDevice.WriteBuffer(szByteData, m_nWriteLength, &nActualLength) == FALSE) {
			MessageBox("Write device failed", "Error", MB_OK | MB_ICONERROR);
		} else {
			MessageBox("Data has been written into device.", "GenericHID", MB_OK | MB_ICONINFORMATION);
		}
	}
	delete(szByteData);
}

LRESULT CGenericHIDDlg::OnDisplayReadData(WPARAM wParam, LPARAM lParam)
{
	CString strByteData;

	//
    // LParam is the device that was read from
    // 
    PHID_DEVICE pDevice = (PHID_DEVICE)lParam;

	for(WORD wCount=0; wCount<pDevice->Caps.InputReportByteLength; wCount++) {
		strByteData.Format("%02x ",pDevice->InputReportBuffer[wCount]);
		m_strInputDisplay += strByteData;
	}
	strByteData.Format("\r\n");
	m_strInputDisplay += strByteData;
	UpdateData(FALSE);
    
    SetEvent( m_ReadContext.DisplayEvent );
	return TRUE;
}

void CGenericHIDDlg::UpdateUI(BOOL bEnable)
{
	CButton* pButton = (CButton*) GetDlgItem(IDC_INPUT);
	pButton->EnableWindow(bEnable);

	pButton = (CButton*) GetDlgItem(IDC_OUTPUT);
	pButton->EnableWindow(bEnable);
}

void CGenericHIDDlg::OnClose() 
{
/*	if(m_hHidNotificationHandle) {
		UnregisterDeviceNotification(m_hHidNotificationHandle);
		m_hHidNotificationHandle = NULL;
	}*/

	if(m_bFoundDevice) {
		m_USBHidDevice.CloseDevice();
		m_bFoundDevice = FALSE;
	}

	EndDialog(0);
	CDialog::OnClose();
}

void CGenericHIDDlg::OnDeviceRemoveDone(WPARAM wParam, LPARAM lParam)
{
	MessageBox("Device has been removed.", "GenericHID", MB_OK | MB_ICONINFORMATION);
}

void CGenericHIDDlg::OnDeviceFindDone(WPARAM wParam, LPARAM lParam)
{
	MessageBox("HID Device found", "GenericHID", MB_OK | MB_ICONINFORMATION);
}

void CGenericHIDDlg::OnUnregisterHandle(WPARAM wParam, LPARAM lParam)
{
	UnregisterDeviceNotification ( (HDEVNOTIFY) lParam ); 
}

BOOL CGenericHIDDlg::OnDeviceChange(WPARAM wParam, LPARAM lParam)
{
    PDEV_BROADCAST_HDR broadcastHdr;

	switch (wParam) {
	case DBT_DEVICEARRIVAL:
		broadcastHdr = (PDEV_BROADCAST_HDR) lParam;
        if ( (DBT_DEVTYP_DEVICEINTERFACE == broadcastHdr -> dbch_devicetype) && !m_bFoundDevice)
        {
            PDEV_BROADCAST_DEVICEINTERFACE  broadcastInterface;
            
            broadcastInterface = (PDEV_BROADCAST_DEVICEINTERFACE) lParam;

            //
            // Open the hid device for query access
            //
            
			if(m_USBHidDevice.FindDeviceByPath(broadcastInterface->dbcc_name, m_hWnd)) {
				m_bFoundDevice = TRUE;
				UpdateUI(TRUE);
		        PostMessage(WM_DEVICE_FIND_DONE, 0, 0);
            } 
	    }
        break;

    case DBT_DEVICEREMOVEPENDING:
    case DBT_DEVICEREMOVECOMPLETE:

        //
        // Do the same steps for DBT_DEVICEREMOVEPENDING and 
        //   DBT_DEVICEREMOVECOMPLETE.  We do not receive the 
        //   remove complete request for a device if it is
        //   disabled or removed via Device Manager.  However,
        //   in that case will receive the remove pending.  
        //   We remove the device from our currently displayed
        //   list of devices and unregister notification.
        //
        broadcastHdr = (PDEV_BROADCAST_HDR) lParam;

        if (DBT_DEVTYP_HANDLE == broadcastHdr -> dbch_devicetype)
        {
            PDEV_BROADCAST_HANDLE broadcastHandle;
            HANDLE                deviceHandle;
            
            broadcastHandle = (PDEV_BROADCAST_HANDLE) lParam;

            //
            // Get the file handle of the device that was removed
            //  from the system
            //
            
            deviceHandle = (HANDLE) broadcastHandle -> dbch_handle;

			if(m_USBHidDevice.CloseDeviceByHandle(deviceHandle, m_hWnd)) {
				UpdateUI(FALSE);
				m_bFoundDevice = FALSE;
            }
        }

		break;

    case DBT_DEVICEQUERYREMOVE:

        //
        // If this message is received, the device is either
        //  being disabled or removed through device manager.
        //  To properly handle this request, we need to close
        //  the handle to the device.
        //

        broadcastHdr = (PDEV_BROADCAST_HDR) lParam;

        if (DBT_DEVTYP_HANDLE == broadcastHdr -> dbch_devicetype)
        {
            PDEV_BROADCAST_HANDLE broadcastHandle;
            HANDLE                deviceHandle;
            
            broadcastHandle = (PDEV_BROADCAST_HANDLE) lParam;

            //
            // Get the file handle of the device that was removed
            //  from the system
            //
            
            deviceHandle = (HANDLE) broadcastHandle -> dbch_handle;

			m_USBHidDevice.CloseDeviceByHandle(deviceHandle, m_hWnd);
        }
        return TRUE;
	}      
	return FALSE;
}

void CGenericHIDDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CGenericHIDDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}
