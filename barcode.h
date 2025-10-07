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
#include <gph-font.h>

#include <win.h>

#include "resource.h"

#define APP_COPYRIGHT               2025
#define APP_PATH                    L"parkit.dat"
#define APP_TITLE                   L"PARKIT"
#define APP_VERSION                 L"0.2.7"

// include needed libraries
#pragma comment(lib, "UTIL.lib")
#pragma comment(lib, "GPH.lib")
#pragma comment(lib, "WIN.lib")

WNDW    wnd;

/**
  @fn               BOOL Server(const BOOL startup)
  @brief            start application as a server 
  @param[in]        startup if TRUE, start the server, if FALSE, destroy
  @return           TRUE if setup/destruction was successful, FALSE otherwise
*/

#endif
