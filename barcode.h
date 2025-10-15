/**
  @file     barcode.h
  @brief    Header file for BARCODE application
  @author   suendisra
  @version  0.0.0
*/
#ifndef _BARCODE_H_
#define _BARCODE_H_

#include <util.h>
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

WNDW    wnd;

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
