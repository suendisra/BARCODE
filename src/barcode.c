/**
  @file     barcode.c
  @brief    Source file for BARCODE application
*/
#include "barcode.h"
#include "resource.h"

// static prototypes
static INT_PTR CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

/* wWinMain */
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previnst, LPWSTR cmd, int show)
{
    // initialize for GUI application
    WinInit(instance, previnst, cmd, show);

    // create window and loop it
    Dims(NULL, 0, NULL, &wnd.client);
    if(Dialog(IDD_BARCODE, NULL, &wnd))
    {
        // loop application window procedure
        WindowConfig(Standup, NULL, Shutdown, &wnd);
        Loop(wnd);
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
        case WM_PAINT:
            // whenever the dialog is repainted, we need to repaint the barcode too
            if(GphPaint(gph, hwnd) == TRUE)
            {
                DrawBarcode();
                GphPaint(gph, hwnd);
            }
            break;

        case WM_COMMAND:
            switch(LOWORD(wp))
            {
                case IDC_BARCODE_TYPE:
                    switch(HIWORD(wp))
                    {
                        case LBN_SELCHANGE:
                            // whenever there is a change to the barcode type, reflect dialog changes
                            EnableOptions(hwnd);
                            break;
                    }
                    break;

                case IDOK:
                    // user has set in the barcode information, obtain it
                    if(ConfigBarcode(hwnd) == TRUE)
                    {
                        // force a window redraw to produce new barcode
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    break;

                case IDCANCEL:
                    SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
                    break;
            }
            break;
    }

    return(rval);
}
