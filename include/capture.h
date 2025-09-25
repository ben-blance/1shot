#ifndef CAPTURE_H
#define CAPTURE_H
#include <windows.h>

HBITMAP CaptureScreen(void);
void CopyToClipboard(HBITMAP hBitmap);

#endif
