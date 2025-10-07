/**
  @file     barcode.c
  @brief    Source file for BARCODE application
  @author   suendisra
*/
#include "barcode.h"

// static prototypes
static INT_PTR CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

/* wWinMain */
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previnst, LPWSTR cmd, int show)
{
    WinInit(instance, previnst, cmd, show);

    // create window and loop it
    Dims(NULL, 0, NULL, &wnd.client);
    if(Dialog(MainDlgProc, IDD_BARCODE, NULL, &wnd) == TRUE)
    {
        // loop application window procedure
        wnd.mode = LOOP_GET;
        Loop(&wnd);
    }

    return(0);
}

/**
  @fn           static INT_PTR CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
  @brief        Main window procedure
  @param[in]    hwnd handle to window whose proc is being processed
  @param[in]    msg windows message to be processed
  @param[in]    wp wparam associated with the message
  @param[in]    lp lparam associated with the message
  @return       system return value
*/
INT_PTR CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    long    rval = 0;

    switch(msg)
    {
        case WM_INITDIALOG:
            // set dialog caption and center on screen
            TextSet(hwnd, 0, L"%s %s", APP_TITLE, APP_VERSION);
            Center(hwnd, 0, NULL);
            break;

        case WM_SYSCOMMAND:
            switch(LOWORD(wp))
            {
                case SC_CLOSE:
                case SC_DEFAULT:
                    Kill(hwnd);
                    break;

                default:
                    rval = DefWindowProc(hwnd, msg, wp, lp);
                    break;
            }
            break;

        case WM_COMMAND:
            switch(LOWORD(wp))
            {
                case IDCANCEL:
                    SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
                    break;
            }
            break;
    }

    return(rval);
}
