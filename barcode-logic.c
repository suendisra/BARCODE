#include "barcode.h"

#define BARCODE_FONT_NAME   L"Consolas"
#define BARCODE_FONT_SIZE   14

static GPH      gph = {0};
static INDEX    font = -1;

// static prototypes
static void DrawBarcode(void);
static void SetupDialog(void);
static BOOL SetupGDI(void);

/* draw the background for the barcode area */
void DrawBarcode(void)
{
    GphClear(gph, NULL, GGRAY);

    // TODO

    GphBlit(gph);
}

/* configure dialog field controls */
void SetupDialog(void)
{
    const wchar_t   bc[BARCODE_LAST][STR_SMALL] = {
        L"BARCODE 39", L"BARCODE 39 MOD 43", L"BARCODE 93",
        L"CODABAR", L"I2of5", L"UPCA", L"UPCE"
    };

    long    n = 0;

    // add in all barcode types and select first one
    for(n = 0; (n < BARCODE_LAST); ++n)
    {
        ListInsert(wnd.handl, IDC_BARCODE_TYPE, -1, L"%s", bc[n]);
    }
    ListSelect(wnd.handl, IDC_BARCODE_TYPE, 0, 0);

    // stretch out the bitmap display area
    SendDlgItemMessage(wnd.handl, IDC_BARCODE, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)gph.mem.bmp);
}

/* helper function to initialize win32 graphics */
BOOL SetupGDI(void)
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
    if(Resource(RSC_TYPE_ICO, IDI_BARCODE, &wnd.icon, sizeof(wnd.icon)) == TRUE)
    {
        WindowIcon(wnd.handl, 0, wnd.icon);

        // stand up GDI graphics
        if(SetupGDI() == TRUE)
        {
            // configure the fields in the dialog
            SetupDialog();
            DrawBarcode();
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
