#ifndef HID_CLIENT_H
#define HID_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hid.h"


#define WM_UNREGISTER_HANDLE    WM_USER+1
#define WM_DISPLAY_READ_DATA    WM_USER+2
#define WM_READ_DONE            WM_USER+3
#define WM_DEVICE_REMOVE_DONE	WM_USER+4
#define WM_DEVICE_FIND_DONE		WM_USER+5

#define READ_THREAD_TIMEOUT     1000

#define HCLIENT_ERROR           "HClient Error"

typedef struct _READ_THREAD_CONTEXT 
{
    PHID_DEVICE HidDevice;

    HWND        DisplayWindow;
    HANDLE      DisplayEvent;

    BOOL        DoOneRead;
    BOOL        TerminateThread;

} READ_THREAD_CONTEXT, *PREAD_THREAD_CONTEXT;

DWORD WINAPI AsynchReadThreadProc(PREAD_THREAD_CONTEXT Context);
BOOLEAN ReadOverlapped(PHID_DEVICE HidDevice, HANDLE CompletionEvent);
BOOLEAN Write(PHID_DEVICE HidDevice);
BOOL ReadHID(PHID_DEVICE HidDevice, PUCHAR pucBuffer, int nReadLength, int *nActualLength);
BOOL WriteHID(PHID_DEVICE HidDevice, PCHAR pucBuffer, int nWriteLength, int *nActualLength);

#ifdef __cplusplus
}
#endif

#endif