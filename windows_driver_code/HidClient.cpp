#include "stdafx.h"
#include "hid.h"
#include "HidClient.h"

BOOL WriteHID(PHID_DEVICE HidDevice, PCHAR pucBuffer, int nWriteLength, int *nActualLength)
{
    DWORD bytesWritten;
	DWORD dwBufCount = 0;

	*nActualLength = 0;
	while(*nActualLength < nWriteLength) {
		for(WORD wCount=1; wCount<HidDevice->Caps.OutputReportByteLength; wCount++) {
			if(dwBufCount < (DWORD)nWriteLength) {
				HidDevice->OutputReportBuffer[wCount] = pucBuffer[dwBufCount++];
			} else {
				HidDevice->OutputReportBuffer[wCount] = 0;
			}
		}

		HidDevice->OutputReportBuffer[0] = 0;
		if (!WriteFile (HidDevice->HidDevice,
				  HidDevice->OutputReportBuffer,
				  HidDevice->Caps.OutputReportByteLength,
				  &bytesWritten,
				  NULL) && (bytesWritten == HidDevice -> Caps.OutputReportByteLength)) 
		{
			return FALSE;
		}

		(*nActualLength) += (HidDevice->Caps.OutputReportByteLength - 1);
	}
	
	return TRUE;
}


/*++
RoutineDescription:
   Given a struct _HID_DEVICE, obtain a read report 
--*/
BOOL ReadHID(PHID_DEVICE HidDevice, PUCHAR pucBuffer, int nReadLength, int *nActualLength)
{
    DWORD    bytesRead;

	*nActualLength = 0;
	while(*nActualLength < nReadLength) {
		if (!ReadFile (HidDevice->HidDevice,
					  HidDevice->InputReportBuffer,
					  HidDevice->Caps.InputReportByteLength,
					  &bytesRead,
					  NULL)) 
		{
			return FALSE;
		}
		for(WORD wCount=1; wCount<HidDevice->Caps.InputReportByteLength; wCount++) {
			int nBufCount = *nActualLength + wCount - 1;
			if(nBufCount >= nReadLength) break;
			pucBuffer[nBufCount] = (UCHAR)HidDevice->InputReportBuffer[wCount];
		}
		(*nActualLength) += (HidDevice->Caps.InputReportByteLength - 1);
		TRACE("Read Length=%x\n", *nActualLength);
	}
	
	return TRUE;
}

DWORD WINAPI AsynchReadThreadProc(PREAD_THREAD_CONTEXT Context)
{
    HANDLE  completionEvent;
    BOOL    readStatus;
    DWORD   waitStatus;
    
    //
    // Create the completion event to send to the the OverlappedRead routine
    //

    completionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    //
    // If NULL returned, then we cannot proceed any farther so we just exit the 
    //  the thread
    //
    
    if (NULL == completionEvent) 
    {
        goto AsyncRead_End;
    }

    //
    // Now we enter the main read loop, which does the following:
    //  1) Calls ReadOverlapped()
    //  2) Waits for read completion with a timeout just to check if 
    //      the main thread wants us to terminate our the read request
    //  3) If the read fails, we simply break out of the loop
    //      and exit the thread
    //  4) If the read succeeds, we call UnpackReport to get the relevant
    //      info and then post a message to main thread to indicate that
    //      there is new data to display.
    //  5) We then block on the display event until the main thread says
    //      it has properly displayed the new data
    //  6) Look to repeat this loop if we are doing more than one read
    //      and the main thread has yet to want us to terminate
    //

    do 
    {
        //
        // Call ReadOverlapped() and if the return status is TRUE, the ReadFile
        //  succeeded so we need to block on completionEvent, otherwise, we just
        //  exit
        //

        readStatus = ReadOverlapped( Context -> HidDevice, completionEvent );

    
        if (!readStatus) 
        {
           break;
        }

        while (!Context -> TerminateThread) 
        {
            //
            // Wait for the completion event to be signaled or a timeout
            //
            
            waitStatus = WaitForSingleObject (completionEvent, READ_THREAD_TIMEOUT );

            //
            // If completionEvent was signaled, then a read just completed
            //   so let's leave this loop and process the data
            //
            
            if ( WAIT_OBJECT_0 == waitStatus)
            { 
                break;
            }
        }

        //
        // Check the TerminateThread again...If it is not set, then data has
        //  been read.  In this case, we want to Unpack the report into our
        //  input info and then send a message to the main thread to display
        //  the new data.
        //
        
        if (!Context -> TerminateThread) 
        {
/*            UnpackReport(Context -> HidDevice -> InputReportBuffer,
                          Context -> HidDevice -> Caps.InputReportByteLength,
                          HidP_Input,
                          Context -> HidDevice -> InputData,
                          Context -> HidDevice -> InputDataLength,
                          Context -> HidDevice -> Ppd);*/
            
            if (NULL != Context -> DisplayEvent) 
            { 
                PostMessage(Context -> DisplayWindow,
                            WM_DISPLAY_READ_DATA,
                            0,
                            (LPARAM) Context -> HidDevice);

                WaitForSingleObject( Context -> DisplayEvent, INFINITE );
            }
        }
    } while ( !Context -> TerminateThread && !Context -> DoOneRead );


AsyncRead_End:

    PostMessage( Context -> DisplayWindow, WM_READ_DONE, 0, 0);
    ExitThread(0);
    return (0);
}

/*++
RoutineDescription:
   Given a struct _HID_DEVICE, obtain a read report and unpack the values
   into the InputData array.
--*/
BOOLEAN ReadOverlapped(PHID_DEVICE HidDevice, HANDLE CompletionEvent)
{
    static OVERLAPPED  overlap;
    DWORD              bytesRead;
    BOOL               readStatus;

    /*
    // Setup the overlap structure using the completion event passed in to
    //  to use for signalling the completion of the Read
    */

    memset(&overlap, 0, sizeof(OVERLAPPED));
    
    overlap.hEvent = CompletionEvent;
    
    /*
    // Execute the read call saving the return code to determine how to 
    //  proceed (ie. the read completed synchronously or not).
    */

    readStatus = ReadFile ( HidDevice -> HidDevice,
                            HidDevice -> InputReportBuffer,
                            HidDevice -> Caps.InputReportByteLength,
                            &bytesRead,
                            &overlap);
                          
    /*
    // If the readStatus is FALSE, then one of two cases occurred.  
    //  1) ReadFile call succeeded but the Read is an overlapped one.  Here,
    //      we should return TRUE to indicate that the Read succeeded.  However,
    //      the calling thread should be blocked on the completion event
    //      which means it won't continue until the read actually completes
    //    
    //  2) The ReadFile call failed for some unknown reason...In this case,
    //      the return code will be FALSE
    */        

    if (!readStatus) 
    {
        return (ERROR_IO_PENDING == GetLastError());
    }

    /*
    // If readStatus is TRUE, then the ReadFile call completed synchronously,
    //   since the calling thread is probably going to wait on the completion
    //   event, signal the event so it knows it can continue.
    */

    else 
    {
        SetEvent(CompletionEvent);
        return (TRUE);
    }
}

BOOLEAN
PackReport (
   OUT PCHAR                ReportBuffer,
   IN  USHORT               ReportBufferLength,
   IN  HIDP_REPORT_TYPE     ReportType,
   IN  PHID_DATA            Data,
   IN  ULONG                DataLength,
   IN  PHIDP_PREPARSED_DATA Ppd
   )
/*++
Routine Description:
   This routine takes in a list of HID_DATA structures (DATA) and builds 
      in ReportBuffer the given report for all data values in the list that 
      correspond to the report ID of the first item in the list.  

   For every data structure in the list that has the same report ID as the first
      item in the list will be set in the report.  Every data item that is 
      set will also have it's IsDataSet field marked with TRUE.

   A return value of FALSE indicates an unexpected error occurred when setting
      a given data value.  The caller should expect that assume that no values
      within the given data structure were set.

   A return value of TRUE indicates that all data values for the given report
      ID were set without error.
--*/
{
    ULONG       numUsages; // Number of usages to set for a given report.
    ULONG       i;
    ULONG       CurrReportID;

    /*
    // All report buffers that are initially sent need to be zero'd out
    */

    memset (ReportBuffer, (UCHAR) 0, ReportBufferLength);

    /*
    // Go through the data structures and set all the values that correspond to
    //   the CurrReportID which is obtained from the first data structure 
    //   in the list
    */

    CurrReportID = Data -> ReportID;

    for (i = 0; i < DataLength; i++, Data++) 
    {
        /*
        // There are two different ways to determine if we set the current data
        //    structure: 
        //    1) Store the report ID were using and only attempt to set those
        //        data structures that correspond to the given report ID.  This
        //        example shows this implementation.
        //
        //    2) Attempt to set all of the data structures and look for the 
        //        returned status value of HIDP_STATUS_INVALID_REPORT_ID.  This 
        //        error code indicates that the given usage exists but has a 
        //        different report ID than the report ID in the current report 
        //        buffer
        */

        if (Data -> ReportID == CurrReportID) 
        {
            if (Data->IsButtonData) 
            {
                numUsages = Data->ButtonData.MaxUsageLength;
                Data->Status = HidP_SetUsages (ReportType,
                                               Data->UsagePage,
                                               0, // All collections
                                               Data->ButtonData.Usages,
                                               &numUsages,
                                               Ppd,
                                               ReportBuffer,
                                               ReportBufferLength);
            }
            else
            {
                Data->Status = HidP_SetUsageValue (ReportType,
                                                   Data->UsagePage,
                                                   0, // All Collections.
                                                   Data->ValueData.Usage,
                                                   Data->ValueData.Value,
                                                   Ppd,
                                                   ReportBuffer,
                                                   ReportBufferLength);
            }

            if (HIDP_STATUS_SUCCESS != Data->Status)
            {
                return FALSE;
            }
        }
    }   

    /*
    // At this point, all data structures that have the same ReportID as the
    //    first one will have been set in the given report.  Time to loop 
    //    through the structure again and mark all of those data structures as
    //    having been set.
    */

    for (i = 0; i < DataLength; i++, Data++) 
    {
        if (CurrReportID == Data -> ReportID)
        {
            Data -> IsDataSet = TRUE;
        }
    }
    return (TRUE);
}



BOOLEAN Write(PHID_DEVICE HidDevice)
{
    DWORD     bytesWritten;
    PHID_DATA pData;
    ULONG     Index;
    BOOLEAN   Status;
    BOOLEAN   WriteStatus;

    /*
    // Begin by looping through the HID_DEVICE's HID_DATA structure and setting
    //   the IsDataSet field to FALSE to indicate that each structure has
    //   not yet been set for this Write call.
    */

    pData = HidDevice -> OutputData;

    for (Index = 0; Index < HidDevice -> OutputDataLength; Index++, pData++) 
    {
        pData -> IsDataSet = FALSE;
    }

    /*
    // In setting all the data in the reports, we need to pack a report buffer
    //   and call WriteFile for each report ID that is represented by the 
    //   device structure.  To do so, the IsDataSet field will be used to 
    //   determine if a given report field has already been set.
    */

    Status = TRUE;

    pData = HidDevice -> OutputData;
    for (Index = 0; Index < HidDevice -> OutputDataLength; Index++, pData++) 
    {

        if (!pData -> IsDataSet) 
        {
            /*
            // Package the report for this data structure.  PackReport will
            //    set the IsDataSet fields of this structure and any other 
            //    structures that it includes in the report with this structure
            */

            PackReport (HidDevice->OutputReportBuffer,
                     HidDevice->Caps.OutputReportByteLength,
                     HidP_Output,
                     pData,
                     HidDevice->OutputDataLength - Index,
                     HidDevice->Ppd);

            /*
            // Now a report has been packaged up...Send it down to the device
            */

            WriteStatus = WriteFile (HidDevice->HidDevice,
                                  HidDevice->OutputReportBuffer,
                                  HidDevice->Caps.OutputReportByteLength,
                                  &bytesWritten,
                                  NULL) && (bytesWritten == HidDevice -> Caps.OutputReportByteLength);

            Status = Status && WriteStatus;                         
        }
    }
    return (Status);
}

