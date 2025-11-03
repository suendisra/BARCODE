/**
  @file     barcode-cli.c
  @brief    Source file for BARCODE application command-line processing
  @author   suendisra
*/
#include "barcode.h"

/* process CLI arguments */
BOOL BarcodeCLI(void)
{
    long    argc = 0;
    LPWSTR  *args = NULL;
    BOOL    handled = FALSE;

    // if CLI arguments were given attempt barcode generation from given parameters
    if(CliArgs(&argc, args) == TRUE)
    {
        // TODO - add in arguments to process
    }

    return(handled);
}