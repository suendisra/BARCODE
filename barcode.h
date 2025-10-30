/**
  @file     barcode.h
  @brief    Header file for BARCODE application
  @author   suendisra
  @version  0.0.0
*/
#ifndef _BARCODE_H_
#define _BARCODE_H_

#include <util.h>
#include <util-cli.h>
#include <util-map.h>
#include <util-thread.h>

#include <gph.h>
#include <gph-barcode.h>
#include <gph-font.h>

#include <win.h>

#include "resource.h"

#define APP_COPYRIGHT               2025
#define APP_TITLE                   L"BARCODE"
#define APP_VERSION                 L"0.0.0"

// include needed libraries
#pragma comment(lib, "UTIL.lib")
#pragma comment(lib, "GPH.lib")
#pragma comment(lib, "WIN.lib")

struct
{
    QUAD                dims;               // drawing dimensions within which to draw

    enum BARCODETYPE    type;               // type of barcode to draw

    COLORREF            fore;               // color of each bar
    COLORREF            back;               // color of the spaces between the bars

    wchar_t             text[STR_MEDIUM];   // text to turn into barcode
    BOOL                drawtext;           // bool indicating whether or not to draw barcode text
}bc;

WNDW    wnd;
GPH     gph;

/**
  @fn           BOOL ConfigBarcode(const HWND hwnd)
  @brief        grabs dialog information for drawing the barcode
  @param[in]    hwnd handle to dialog window with information needed to generate the barcode
*/
BOOL ConfigBarcode(const HWND hwnd);

/**
  @fn           void DrawBarcode(void)
  @brief        draw the background for the barcode area
*/
void DrawBarcode(void);

/**
  @fn           void EnableOptions(const HWND hwnd)
  @brief        enable and/or disable options on the dialog based on barcode type selected
  @param[in]    hwnd handle to dialog window with information needed to generate the barcode
*/
void EnableOptions(const HWND hwnd);

/**
  @fn           BOOL Standup(void)
  @brief        start up application
  @return       true if successful, false otherwise
*/
BOOL Standup(void);

/**
  @fn           void Shutdown(void)
  @brief        stop and tear down application
*/
void Shutdown(void);

#endif
