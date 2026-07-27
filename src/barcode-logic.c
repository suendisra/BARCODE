/**
  @file     barcode-logic.c
  @brief    Source file for BARCODE application primary logic
*/
#include "barcode.h"
#include "resource.h"

#include <gph-font.h>
#include <gph-px.h>

#include <util-mem.h>
#include <util-str.h>

#define BARCODE_ERR_DRAWING L"Error encountered while attempting to draw given barcode text"
#define BARCODE_FONT_NAME   L"Consolas"
#define BARCODE_FONT_SIZE   24

#define BARCODE_PAD_CX      10
#define BARCODE_PAD_CY      20

#define RGB_TEXT_LIMIT      3

static INDEX    font = -1;

// static prototypes
static void GetColor(const HWND hwnd, const BOOL fore, PX24 *clr);
static void SetupDialog(void);
static BOOL SetupGDI(void);

/* grabs dialog information for drawing the barcode */
BOOL ConfigBarcode(const HWND hwnd)
{
    PX24    back = {0};
    PX24    color = {0};

    wchar_t start[STR_TINY] = {0};
    wchar_t text[STR_MEDIUM] = {0};
    wchar_t stop[STR_TINY] = {0};

    BOOL    success = FALSE;

    if(hwnd != NULL)
    {
        // grab the barcode text
        if(TextGet(hwnd, IDC_BARCODE_TEXT, text, sizeof(text)) == TRUE)
        {
            // start code
            if(Enabled(hwnd, IDC_CHAR_START) == TRUE)
            {
                ListItem(hwnd, IDC_CHAR_START, LISTIDX_SEL, start, sizeof(start));
            }

            // stop code
            if(Enabled(hwnd, IDC_CHAR_STOP) == TRUE)
            {
                ListItem(hwnd, IDC_CHAR_STOP, LISTIDX_SEL, stop, sizeof(stop));
            }

            // set in the barcode text
            if(StrFormat(bc.text, sizeof(bc.text), L"%s%s%s", start, text, stop) == TRUE)
            {
                // get the other barcode properties
                bc.type = ListIndex(hwnd, IDC_BARCODE_TYPE, LISTIDX_SEL);
                bc.drawtext = (IsDlgButtonChecked(hwnd, IDC_BARCODE_SHOW_TEXT) == BST_CHECKED);

                // grab color for the bar
                GetColor(hwnd, TRUE, &color);
                bc.fore = RGB(color.r, color.g, color.b);

                // grab color for the space
                GetColor(hwnd, FALSE, &back);
                bc.back = RGB(back.r, back.g, back.b);

                success = TRUE;
            }
        }
    }

    return(success);
}

/* draw the background for the barcode area */
void DrawBarcode(void)
{
    QUAD    dims = {0};
    long    cy = 0;

    GphClear(gph, NULL, GGRAY);

    if(bc.text[0] != CHARNULL)
    {
        // attempt to draw the barcode
        if(Barcode(gph, &bc.dims, bc.type, bc.fore, bc.back, bc.text, sizeof(bc.text)) == TRUE)
        {
            // success, draw text if desired
            if((bc.drawtext == TRUE) && (FontDims(font, GphDC(gph), NULL, &cy, L"%s", bc.text) == TRUE))
            {
                Quad(bc.dims.x1, (bc.dims.y2 - cy), bc.dims.x2, bc.dims.y2, &dims);
                GphFontColor(gph, bc.fore, bc.back);
                GphText(gph, &dims, ALIGN_LEFT, L"%s", bc.text);
            }
        }else{
            // failed, draw informational text
            GphClear(gph, NULL, GGRAY);
            GphFontColor(gph, GWHITE, GGRAY);
            GphText(gph, NULL, ALIGN_CENTER2D, L"%s", BARCODE_ERR_DRAWING);
        }
    }

    GphBlit(gph);
}

/* enable and/or disable options on the dialog based on barcode type selected */
void EnableOptions(const HWND hwnd)
{
    enum BARCODETYPE    type = 0;

    long n = 0;

    if(hwnd != NULL)
    {
        // clear out the start and stop codes
        ListClear(hwnd, IDC_CHAR_STOP);
        ListClear(hwnd, IDC_CHAR_START);

        // default start stop codes to be disabled
        Enable(hwnd, IDC_CHAR_STOP, FALSE);
        Enable(hwnd, IDC_CHAR_START, FALSE);
        Enable(hwnd, IDC_BARCODE_SHOW_TEXT, TRUE);
        CheckDlgButton(hwnd, IDC_BARCODE_SHOW_TEXT, BST_UNCHECKED);

        // determine selected barcode type
        switch(ListIndex(hwnd, IDC_BARCODE_TYPE, LISTIDX_SEL))
        {
            case BARCODE_39:
            case BARCODE_39_MOD43:
                ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BARCODE_START_STOP_39);
                ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BARCODE_START_STOP_39);
                break;

            case BARCODE_93:
                ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BARCODE_START_STOP_93);
                ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BARCODE_START_STOP_93);
                break;

            case BARCODE_CODA:
                Enable(hwnd, IDC_CHAR_STOP, TRUE);
                Enable(hwnd, IDC_CHAR_START, TRUE);
                for(n = 0; (n < StrLen(BARCODE_START_STOP_CODA, (STR_TINY * sizeof(wchar_t)))); ++n)
                {
                    ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BARCODE_START_STOP_CODA[n]);
                    ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BARCODE_START_STOP_CODA[n]);
                }
                break;

            case BARCODE_I2OF5:
                ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BARCODE_START_I2OF5);
                ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BARCODE_STOP_I2OF5);
                for(n = 0; (n < StrLen(BARCODE_START_STOP_UPC, (STR_TINY * sizeof(wchar_t)))); ++n)
                {
                    ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BARCODE_START_STOP_UPC[n]);
                    ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BARCODE_START_STOP_UPC[n]);
                }
                break;

            case BARCODE_UPCA:
            case BARCODE_UPCE:
                Enable(hwnd, IDC_CHAR_STOP, TRUE);
                Enable(hwnd, IDC_CHAR_START, TRUE);
                Enable(hwnd, IDC_BARCODE_SHOW_TEXT, FALSE);
                CheckDlgButton(hwnd, IDC_BARCODE_SHOW_TEXT, BST_CHECKED);
                for(n = 0; (n < StrLen(BARCODE_START_STOP_UPC, (STR_TINY * sizeof(wchar_t)))); ++n)
                {
                    ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BARCODE_START_STOP_UPC[n]);
                    ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BARCODE_START_STOP_UPC[n]);
                }
                break;
        }

        // select first item for start/stop codes
        ListSelect(hwnd, IDC_CHAR_START, 0, 0);
        ListSelect(hwnd, IDC_CHAR_STOP, 0, 0);
    }
}

/* helper function to reach into dialog controls and return PX24 object */
static void GetColor(const HWND hwnd, const BOOL fore, PX24 *clr)
{
    const long  idR = ((fore == TRUE) ? IDC_BARCODE_COLOR_R : IDC_BARCODE_BACK_R);
    const long  idG = ((fore == TRUE) ? IDC_BARCODE_COLOR_G : IDC_BARCODE_BACK_G);
    const long  idB = ((fore == TRUE) ? IDC_BARCODE_COLOR_B : IDC_BARCODE_BACK_B);

    long    val = 0;

    wchar_t temp[STR_TINY] = {0};

    if((hwnd != NULL) && (clr != NULL))
    {
        // clear out any values that may already be in the PX24 object
        MemClear(clr, sizeof(*clr));

        // get red value
        if(TextGet(hwnd, idR, temp, sizeof(temp)))
        {
            Convert(CONVERT_STR_LNG, &val, temp, sizeof(val));
            clr->r = (BYTE)Clamp(val, 0, 255);
        }

        // get green value
        if(TextGet(hwnd, idG, temp, sizeof(temp)))
        {
            Convert(CONVERT_STR_LNG, &val, temp, sizeof(val));
            clr->g = (BYTE)Clamp(val, 0, 255);
        }

        // get blue value
        if(TextGet(hwnd, idB, temp, sizeof(temp)))
        {
            Convert(CONVERT_STR_LNG, &val, temp, sizeof(val));
            clr->b = (BYTE)Clamp(val, 0, 255);
        }
    }
}

/* configure dialog field controls */
static void SetupDialog(void)
{
    const wchar_t   names[BARCODE_LAST][STR_SMALL] = {
        L"BARCODE 39", L"BARCODE 39 MOD 43", L"BARCODE 93",
        L"CODABAR", L"I2of5", L"UPCA", L"UPCE"
    };

    PX24    back = {0};
    PX24    color = {0};

    long    n = 0;

    // set dialog caption and center on screen
    TextSet(wnd.handl, 0, L"%s %s", APP_TITLE, APP_VERSION);
    Center(wnd.handl, 0, NULL);

    // get dimensions of drawing area
    Dims(wnd.handl, IDC_BARCODE, NULL, &bc.dims);

    // add in all barcode types and select first one
    for(n = 0; (n < BARCODE_LAST); ++n)
    {
        ListInsert(wnd.handl, IDC_BARCODE_TYPE, -1, L"%s", names[n]);
    }
    ListSelect(wnd.handl, IDC_BARCODE_TYPE, 0, 0);

    // limit text input for RGB values
    EditLimit(wnd.handl, IDC_BARCODE_BACK_R, RGB_TEXT_LIMIT);
    EditLimit(wnd.handl, IDC_BARCODE_BACK_G, RGB_TEXT_LIMIT);
    EditLimit(wnd.handl, IDC_BARCODE_BACK_B, RGB_TEXT_LIMIT);
    EditLimit(wnd.handl, IDC_BARCODE_COLOR_R, RGB_TEXT_LIMIT);
    EditLimit(wnd.handl, IDC_BARCODE_COLOR_G, RGB_TEXT_LIMIT);
    EditLimit(wnd.handl, IDC_BARCODE_COLOR_B, RGB_TEXT_LIMIT);

    // grab the colors for the barcode and layout on the dialog
    Px24(bc.back, &back);
    Px24(bc.fore, &color);

    // space color
    TextSet(wnd.handl, IDC_BARCODE_BACK_R, L"%03d", back.r);
    TextSet(wnd.handl, IDC_BARCODE_BACK_G, L"%03d", back.g);
    TextSet(wnd.handl, IDC_BARCODE_BACK_B, L"%03d", back.b);

    // car color
    TextSet(wnd.handl, IDC_BARCODE_COLOR_R, L"%03d", color.r);
    TextSet(wnd.handl, IDC_BARCODE_COLOR_G, L"%03d", color.g);
    TextSet(wnd.handl, IDC_BARCODE_COLOR_B, L"%03d", color.b);

    // stretch out the bitmap display area
    SendDlgItemMessage(wnd.handl, IDC_BARCODE, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GphBmp(gph));
}

/* helper function to initialize win32 graphics */
static BOOL SetupGDI(void)
{
    QUAD    dims = {0};
    QUAD    ctrl = {0};

    BOOL    success = FALSE;

    // adjust dialog client dimensions for password list
    if(Dims(wnd.handl, IDC_BARCODE_CONTROL, &ctrl, NULL) == TRUE)
    {
        // stand up the graphics
        Quad(0, 0, (wnd.client.cx - ctrl.x2), ctrl.cy, &dims);
        if(Gph(GetDlgItem(wnd.handl, IDC_BARCODE), dims, &gph) == TRUE)
        {
            // create the font needed for the password list
            font = FontGDI(BARCODE_FONT_NAME, BARCODE_FONT_SIZE, TRUE, FALSE, FALSE);
            if(font >= 0)
            {
                // assign the font
                GphFont(&gph, font);

                // set up some default barcode properties
                MemClear(&bc, sizeof(bc));
                QuadCentered(gph.client.mx, gph.client.my, (gph.client.cx - (2 * BARCODE_PAD_CX)), (gph.client.cy - (2 * BARCODE_PAD_CY)), &bc.dims);
                bc.fore = GBLACK;
                bc.back = GWHITE;
                success = TRUE;
            }
        }
    }

    return(success);
}

/* start up application */
BOOL Standup(void)
{
    BOOL    success = FALSE;

    // load and set application icon
    if(WindowIcon(IDI_BARCODE, &wnd))
    {
        // stand up GDI graphics
        if(SetupGDI())
        {
            // configure the fields in the dialog
            SetupDialog();
            EnableOptions(wnd.handl);
            success = TRUE;
        }
    }

    return(success);
}

/* stop and tear down application */
void Shutdown(void)
{
    FontKillGDI(font);
    GphKill(&gph);
}
