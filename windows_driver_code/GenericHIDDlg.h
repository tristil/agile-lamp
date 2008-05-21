// GenericHIDDlg.h : header file
//

#if !defined(AFX_GENERICHIDDLG_H__18368FF8_0F01_40AC_89DC_C59E95EC3124__INCLUDED_)
#define AFX_GENERICHIDDLG_H__18368FF8_0F01_40AC_89DC_C59E95EC3124__INCLUDED_

#include "HidClient.h"	// Added by ClassView
#include "hid.h"	// Added by ClassView
#include "USBHID.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGenericHIDDlg dialog

class CGenericHIDDlg : public CDialog
{
// Construction
public:
	USBHID m_USBHidDevice;
	BOOL m_bFoundDevice;
	HANDLE m_hHidNotificationHandle;
	void UpdateUI(BOOL bEnable);
	READ_THREAD_CONTEXT m_ReadContext;
	CGenericHIDDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGenericHIDDlg)
	enum { IDD = IDD_GENERICHID_DIALOG };
	int		m_nOutputPattern;
	CString	m_strInputDisplay;
	int		m_nReadLength;
	int		m_nWriteLength;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenericHIDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGenericHIDDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnInput();
	afx_msg void OnOutput();
	afx_msg LRESULT OnDisplayReadData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg BOOL OnDeviceChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUnregisterHandle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDeviceRemoveDone(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDeviceFindDone(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERICHIDDLG_H__18368FF8_0F01_40AC_89DC_C59E95EC3124__INCLUDED_)
