/**
  @file     barcode.c
  @brief    Source file for BARCODE application
*/
#include "barcode.h"
#include "resource.h"

/* wWinMain */
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previnst, LPWSTR cmd, int show)
{
    // initialize for GUI application
    WinInit(instance, previnst, cmd, show);

    // create window and loop it
    Dims(NULL, 0, NULL, &wnd.client);
    if(Dialog(IDD_BAR, NULL, &wnd))
    {
        // loop application window procedure
        WindowConfig(Standup, NULL, Shutdown, &wnd);
        WindowFunc(WM_PAINT, (void*)DrawBarcode, &wnd);
        WindowFunc(WM_COMMAND, (void*)Commands, &wnd);
        Loop(wnd);
    }

    return(0);
}
