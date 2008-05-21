// USBHID.h: interface for the USBHID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USBHID_H__1FBB32BD_44D4_48F6_84B4_0CE69A02A578__INCLUDED_)
#define AFX_USBHID_H__1FBB32BD_44D4_48F6_84B4_0CE69A02A578__INCLUDED_

#include "hid.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class USBHID  
{
public:
	BOOL ReadBuffer(PUCHAR pucBuffer, int nReadLength, int *nActualLength);
	BOOL FindDeviceByPath(PCHAR szDevicePath, HWND hWnd);
	BOOL CloseDeviceByHandle(HANDLE hDevice, HWND hWnd);
	void CloseDevice();
	HID_DEVICE m_HidDevice;
	HID_DEVICE m_HidCBWDevice;
	BOOL FindDevice(HWND hWnd);
	BOOL WriteBuffer(PCHAR pcBuffer, int nWriteLength, int *nActualLength);
	USBHID();
	virtual ~USBHID();

};

#endif // !defined(AFX_USBHID_H__1FBB32BD_44D4_48F6_84B4_0CE69A02A578__INCLUDED_)
