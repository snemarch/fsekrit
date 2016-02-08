#pragma once
#ifndef id8313AC62_64C9_44FD_8B8694B0EC00F1B7
#define id8313AC62_64C9_44FD_8B8694B0EC00F1B7

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/************************************************************************/
/* From "Icons in Win32" by John Hornick, Microsoft Corporation, 1995   */
/* found on www.wotsit.org                                              */
/************************************************************************/

// The GRP* structures are for icons in PE executable resources
#pragma pack(push,2)
struct GRPICONDIRENTRY
{
   BYTE   bWidth;               // Width, in pixels, of the image
   BYTE   bHeight;              // Height, in pixels, of the image
   BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
   BYTE   bReserved;            // Reserved
   WORD   wPlanes;              // Color Planes
   WORD   wBitCount;            // Bits per pixel
   DWORD  dwBytesInRes;         // how many bytes in this resource?
   WORD   nID;                  // the ID
};


struct GRPICONDIR
{
   WORD            idReserved;   // Reserved (must be 0)
   WORD            idType;       // Resource type (1 for icons)
   WORD            idCount;      // How many images?
//   GRPICONDIRENTRY   idEntries[1]; // The entries for each image
};
#pragma pack( pop )


// the ICON* structures are for icons in .ico files
#pragma pack (push,1)
struct ICONDIRENTRY
{
    BYTE        bWidth;          // Width, in pixels, of the image
    BYTE        bHeight;         // Height, in pixels, of the image
    BYTE        bColorCount;     // Number of colors in image (0 if >=8bpp)
    BYTE        bReserved;       // Reserved ( must be 0)
    WORD        wPlanes;         // Color Planes
    WORD        wBitCount;       // Bits per pixel
    DWORD       dwBytesInRes;    // How many bytes in this resource?
    DWORD       dwImageOffset;   // Where in the file is this image?
};


struct ICONDIR
{
    WORD           idReserved;   // Reserved (must be 0)
    WORD           idType;       // Resource Type (1 for icons)
    WORD           idCount;      // How many images?
//    ICONDIRENTRY   idEntries[1]; // An entry for each image (idCount of 'em)
};
#pragma pack(pop)

#endif // header
