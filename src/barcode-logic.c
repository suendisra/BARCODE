/**
  @file     barcode-logic.c
  @brief    Source file for BARCODE application primary logic
*/
#include "barcode.h"
#include "resource.h"

#include <gph-bar.h>
#include <gph-font.h>
#include <gph-px.h>
#include <gph-std.h>

#include <util-mem.h>
#include <util-str.h>

#define BAR_ERR_DRAWING     L"Error encountered while attempting to draw given barcode text"
#define BAR_FONT_NAME       L"Consolas"
#define BAR_FONT_SIZE       24

#define BAR_PAD_CX          10
#define BAR_PAD_CY          20

#define RGB_TEXT_LIMIT      3

enum BARCODES
{
    BAR_INVAL = -1,
    BAR_39,
    BAR_39_MOD43,
    BAR_93,
    BAR_CODA,
    BAR_I2OF5,
    BAR_UPCA,
    BAR_UPCE,
    BAR_LAST
};

static const wchar_t    barcodes[BAR_LAST][STR_SMALL] = {
                            L"BARCODE 39",
                            L"BARCODE 39 MOD 43",
                            L"BARCODE 93",
                            L"CODABAR",
                            L"I2of5",
                            L"UPCA",
                            L"UPCE"
                        };

static INDEX            font = -1;

// static prototypes
static BOOL ConfigBarcode(const HWND hwnd);
static void EnableOptions(const HWND hwnd);
static void GetColor(const HWND hwnd, const BOOL fore, PX24 *clr);
static void SetupDialog(void);
static BOOL SetupGDI(void);

/* handle user command given to the GUI */
BOOL Commands(const HWND hwnd, const WPARAM wp, const LPARAM lp)
{
    BOOL    handled = FALSE;

    if(lp != 0){}
    switch(LOWORD(wp))
    {
        case IDC_BAR_TYPE:
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
            if(ConfigBarcode(hwnd))
            {
                // force a window redraw to produce new barcode
                Refresh(hwnd);
            }
            break;
    }

    return(handled);
}

/* grabs dialog information for drawing the barcode */
static BOOL ConfigBarcode(const HWND hwnd)
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
        if(TextGet(hwnd, IDC_BAR_TEXT, text, sizeof(text)))
        {
            // start code
            if(Enabled(hwnd, IDC_CHAR_START))
            {
                ListItem(hwnd, IDC_CHAR_START, LISTIDX_SEL, start, sizeof(start));
            }

            // stop code
            if(Enabled(hwnd, IDC_CHAR_STOP))
            {
                ListItem(hwnd, IDC_CHAR_STOP, LISTIDX_SEL, stop, sizeof(stop));
            }

            // set in the barcode text
            if(StrFormat(bc.text, sizeof(bc.text), L"%s%s%s", start, text, stop))
            {
                // get the other barcode properties
                bc.type = ListIndex(hwnd, IDC_BAR_TYPE, LISTIDX_SEL);
                bc.drawtext = ButtonChecked(hwnd, IDC_BAR_SHOW_TEXT);

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
void DrawBarcode(const HWND hwnd)
{
    QUAD    dims = {0};
    long    cy = 0;
    long    len = 0;
    BOOL    drawn = FALSE;

    if(GphPaint(gph, hwnd))
    {
        GphClear(gph, NULL, GGRAY);
        if(bc.text[0] != CHARNULL)
        {
            // attempt to draw the barcode
            len = StrLen(bc.text, sizeof(bc.text));
            switch(bc.type)
            {
                case BAR_39:
                case BAR_39_MOD43:
                    drawn = Bar39(gph, bc.dims, bc.fore, bc.back, (bc.type == BAR_39_MOD43), bc.text, len);
                    break;

                case BAR_93:
                    drawn = Bar93(gph, bc.dims, bc.fore, bc.back, bc.text, len);
                    break;

                case BAR_CODA:
                    drawn = BarCoda(gph, bc.dims, bc.fore, bc.back, bc.text, len);
                    break;

                case BAR_I2OF5:
                    drawn = BarI2of5(gph, bc.dims, bc.fore, bc.back, bc.text, len);
                    break;

                case BAR_UPCA:
                    drawn = BarUPCA(gph, bc.dims, bc.fore, bc.back, bc.text, len);
                    break;

                case BAR_UPCE:
                    drawn = BarUPCE(gph, bc.dims, bc.fore, bc.back, bc.text, len);
                    break;
            }

            if(drawn)
            {
                // success, draw text if desired
                if(bc.drawtext && FontDims(font, GphDC(gph), NULL, &cy, L"%s", bc.text))
                {
                    Quad(bc.dims.x1, (bc.dims.y2 - cy), bc.dims.x2, bc.dims.y2, &dims);
                    GphFontColor(gph, bc.fore, bc.back);
                    GphText(gph, &dims, ALIGN_LEFT, L"%s", bc.text);
                }
            }else{
                // failed, draw informational text
                GphClear(gph, NULL, GGRAY);
                GphFontColor(gph, GWHITE, GGRAY);
                GphText(gph, NULL, ALIGN_CENTER2D, L"%s", BAR_ERR_DRAWING);
            }
        }

        GphBlit(gph);
        GphPaint(gph, hwnd);
    }
}

/* enable and/or disable options on the dialog based on barcode type selected */
static void EnableOptions(const HWND hwnd)
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
        Enable(hwnd, IDC_BAR_SHOW_TEXT, TRUE);
        CheckDlgButton(hwnd, IDC_BAR_SHOW_TEXT, BST_UNCHECKED);

        // determine selected barcode type
        switch(ListIndex(hwnd, IDC_BAR_TYPE, LISTIDX_SEL))
        {
            case BAR_39:
            case BAR_39_MOD43:
                ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BAR_START_STOP_39);
                ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BAR_START_STOP_39);
                break;

            case BAR_93:
                ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BAR_START_STOP_93);
                ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BAR_START_STOP_93);
                break;

            case BAR_CODA:
                Enable(hwnd, IDC_CHAR_STOP, TRUE);
                Enable(hwnd, IDC_CHAR_START, TRUE);
                for(n = 0; (n < StrLen(BAR_START_STOP_CODA, (STR_TINY * sizeof(wchar_t)))); ++n)
                {
                    ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BAR_START_STOP_CODA[n]);
                    ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BAR_START_STOP_CODA[n]);
                }
                break;

            case BAR_I2OF5:
                ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BAR_START_I2OF5);
                ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BAR_STOP_I2OF5);
                for(n = 0; (n < StrLen(BAR_START_STOP_UPC, (STR_TINY * sizeof(wchar_t)))); ++n)
                {
                    ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BAR_START_STOP_UPC[n]);
                    ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BAR_START_STOP_UPC[n]);
                }
                break;

            case BAR_UPCA:
            case BAR_UPCE:
                Enable(hwnd, IDC_CHAR_STOP, TRUE);
                Enable(hwnd, IDC_CHAR_START, TRUE);
                Enable(hwnd, IDC_BAR_SHOW_TEXT, FALSE);
                CheckDlgButton(hwnd, IDC_BAR_SHOW_TEXT, BST_CHECKED);
                for(n = 0; (n < StrLen(BAR_START_STOP_UPC, (STR_TINY * sizeof(wchar_t)))); ++n)
                {
                    ListInsert(hwnd, IDC_CHAR_START, LISTIDX_INVAL, L"%c", BAR_START_STOP_UPC[n]);
                    ListInsert(hwnd, IDC_CHAR_STOP, LISTIDX_INVAL, L"%c", BAR_START_STOP_UPC[n]);
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
    const long  idR = ((fore == TRUE) ? IDC_BAR_COLOR_R : IDC_BAR_BACK_R);
    const long  idG = ((fore == TRUE) ? IDC_BAR_COLOR_G : IDC_BAR_BACK_G);
    const long  idB = ((fore == TRUE) ? IDC_BAR_COLOR_B : IDC_BAR_BACK_B);

    long    val = 0;

    wchar_t temp[STR_TINY] = {0};

    if((hwnd != NULL) && (clr != NULL))
    {
        // clear out any values that may already be in the PX24 object
        MemClear(clr, sizeof(*clr));

        // get red value
        if(TextGet(hwnd, idR, temp, sizeof(temp)))
        {
            clr->r = (BYTE)Clamp(ToLong(temp), 0, 255);
        }

        // get green value
        if(TextGet(hwnd, idG, temp, sizeof(temp)))
        {
            clr->g = (BYTE)Clamp(ToLong(temp), 0, 255);
        }

        // get blue value
        if(TextGet(hwnd, idB, temp, sizeof(temp)))
        {
            clr->b = (BYTE)Clamp(ToLong(temp), 0, 255);
        }
    }
}

/* configure dialog field controls */
static void SetupDialog(void)
{
    PX24    back = {0};
    PX24    color = {0};

    long    n = 0;

    // set dialog caption and center on screen
    TextSet(wnd.handl, 0, L"%s %s", APP_TITLE, APP_VERSION);
    Center(wnd.handl, 0, NULL);

    // get dimensions of drawing area
    Dims(wnd.handl, IDC_BAR, NULL, &bc.dims);

    // add in all barcode types and select first one
    for(n = 0; (n < BAR_LAST); ++n)
    {
        ListInsert(wnd.handl, IDC_BAR_TYPE, -1, L"%s", barcodes[n]);
    }
    ListSelect(wnd.handl, IDC_BAR_TYPE, 0, 0);

    // limit text input for RGB values
    EditLimit(wnd.handl, IDC_BAR_BACK_R, RGB_TEXT_LIMIT);
    EditLimit(wnd.handl, IDC_BAR_BACK_G, RGB_TEXT_LIMIT);
    EditLimit(wnd.handl, IDC_BAR_BACK_B, RGB_TEXT_LIMIT);
    EditLimit(wnd.handl, IDC_BAR_COLOR_R, RGB_TEXT_LIMIT);
    EditLimit(wnd.handl, IDC_BAR_COLOR_G, RGB_TEXT_LIMIT);
    EditLimit(wnd.handl, IDC_BAR_COLOR_B, RGB_TEXT_LIMIT);

    // grab the colors for the barcode and layout on the dialog
    Px24(bc.back, &back);
    Px24(bc.fore, &color);

    // space color
    TextSet(wnd.handl, IDC_BAR_BACK_R, L"%03d", back.r);
    TextSet(wnd.handl, IDC_BAR_BACK_G, L"%03d", back.g);
    TextSet(wnd.handl, IDC_BAR_BACK_B, L"%03d", back.b);

    // car color
    TextSet(wnd.handl, IDC_BAR_COLOR_R, L"%03d", color.r);
    TextSet(wnd.handl, IDC_BAR_COLOR_G, L"%03d", color.g);
    TextSet(wnd.handl, IDC_BAR_COLOR_B, L"%03d", color.b);

    // stretch out the bitmap display area
    SendDlgItemMessage(wnd.handl, IDC_BAR, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GphBmp(gph));
}

/* helper function to initialize win32 graphics */
static BOOL SetupGDI(void)
{
    QUAD    dims = {0};
    QUAD    ctrl = {0};

    BOOL    success = FALSE;

    // adjust dialog client dimensions for password list
    if(Dims(wnd.handl, IDC_BAR_CONTROL, &ctrl, NULL) == TRUE)
    {
        // stand up the graphics
        Quad(0, 0, (wnd.client.cx - ctrl.x2), ctrl.cy, &dims);
        if(Gph(GetDlgItem(wnd.handl, IDC_BAR), dims, &gph) == TRUE)
        {
            // create the font needed for the password list
            font = FontGDI(BAR_FONT_NAME, BAR_FONT_SIZE, TRUE, FALSE, FALSE);
            if(font >= 0)
            {
                // assign the font
                GphFontSet(gph, font);

                // set up some default barcode properties
                MemClear(&bc, sizeof(bc));
                QuadCentered(gph.client.mx, gph.client.my, (gph.client.cx - (2 * BAR_PAD_CX)), (gph.client.cy - (2 * BAR_PAD_CY)), &bc.dims);
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
    if(WindowIcon(IDI_BAR, &wnd))
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
    WindowKill(&wnd);
}
