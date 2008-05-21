// USBHID.cpp: implementation of the USBHID class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenericHID.h"
#include "USBHID.h"
#include "Hid.h"
#include "HidClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

USBHID::USBHID()
{
	m_HidDevice.bFound = m_HidCBWDevice.bFound = FALSE;
	m_HidDevice.hDeviceNotificationHandle = m_HidCBWDevice.hDeviceNotificationHandle = NULL;
}

USBHID::~USBHID()
{
	CloseDevice();
}

BOOL USBHID::FindDevice(HWND hWnd)
{
	return FindGenericHIDDevice(&m_HidDevice, &m_HidCBWDevice, hWnd);
}

void USBHID::CloseDevice()
{
	if(m_HidDevice.bFound) {
		CloseHidDevice (&m_HidDevice, TRUE);
	}

	if(m_HidCBWDevice.bFound) {
		CloseHidDevice (&m_HidCBWDevice, TRUE);
	}
}

BOOL USBHID::CloseDeviceByHandle(HANDLE hDevice, HWND hWnd)
{
	if(m_HidDevice.HidDevice == hDevice)
    {
        PostMessage(hWnd, WM_UNREGISTER_HANDLE, 0, (LPARAM) m_HidDevice.hDeviceNotificationHandle);
	    CloseHidDevice(&m_HidDevice, TRUE);
	} else if(m_HidCBWDevice.HidDevice == hDevice) {
        PostMessage(hWnd, WM_UNREGISTER_HANDLE, 0, (LPARAM) m_HidCBWDevice.hDeviceNotificationHandle);
	    CloseHidDevice(&m_HidCBWDevice, TRUE);
	}

	
	if(!m_HidDevice.bFound && !m_HidCBWDevice.bFound) {
        PostMessage(hWnd, WM_DEVICE_REMOVE_DONE, 0, 0);
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL USBHID::FindDeviceByPath(PCHAR szDevicePath, HWND hWnd)
{
	BOOL bFoundDevice = FALSE;
	if(!m_HidDevice.bFound) {
		if (OpenHidDevice (szDevicePath,
							TRUE,
							TRUE,
							FALSE,
							FALSE,
							TRUE,
							&m_HidDevice,
							hWnd,
							1, 0)) {	// Find the device
			bFoundDevice = TRUE;
		}
    } 
	if(!bFoundDevice && !m_HidCBWDevice.bFound) {
		if (OpenHidDevice (szDevicePath,
						TRUE,
						TRUE,
						FALSE,
						FALSE,
						TRUE,
						&m_HidCBWDevice,
						hWnd,
						1, 3)) {	// Find the CBW device
		bFoundDevice = TRUE;
		}
    }

	if(bFoundDevice) {
		if(m_HidDevice.bFound && m_HidCBWDevice.bFound) return TRUE;
		else return FALSE;
	} else {
		return FALSE;
	}
}

BOOL USBHID::ReadBuffer(PUCHAR pucBuffer, int nReadLength, int *nActualLength)
{
	char cCBWBuf[8] = {0x55, 0x53, 0x42, 0x43, 0x00, 0x00, 0x01, 0x00};
	int nReturnedLength;

	if(nReadLength >= 0xffff) return FALSE;

	cCBWBuf[0x04] = (nReadLength & 0xff00) >> 8;
	cCBWBuf[0x05] = nReadLength & 0xff;

	// Write CBW first
	if(WriteHID(&m_HidCBWDevice, cCBWBuf, 8, &nReturnedLength)) {
		return ReadHID(&m_HidDevice, pucBuffer, nReadLength, nActualLength);
	} else {
		return FALSE;
	}
}

BOOL USBHID::WriteBuffer(PCHAR pcBuffer, int nWriteLength, int *nActualLength)
{
	char cCBWBuf[8] = {0x55, 0x53, 0x42, 0x43, 0x00, 0x00, 0x02, 0x00};
	int nReturnedLength;

	if(nWriteLength >= 0xffff) return FALSE;

	cCBWBuf[0x04] = (nWriteLength & 0xff) >> 8;
	cCBWBuf[0x05] = nWriteLength & 0xff;

	// Write CBW first
	if(WriteHID(&m_HidCBWDevice, cCBWBuf, 8, &nReturnedLength)) {
		return WriteHID(&m_HidDevice, pcBuffer, nWriteLength, nActualLength);
	} else {
		return FALSE;
	}
}
