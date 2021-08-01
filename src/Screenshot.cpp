#include <Windows.h>
#include <wingdi.h>
#include <gdiplus.h>

#include "Screenshot.h"

int getEncoderClsid(const WCHAR* format, CLSID* clsid)
{
    UINT num = 0;
    UINT size = 0;

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;

    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *clsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return 0;
}

Screenshot::Screenshot(const std::string& path)
    : path(path)
{
    // Init Gdiplus
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Inside own scope to prevent destruction after GdiplusShutdown
    {
        int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        // Create Contexts
        HWND hDesktopWnd = GetDesktopWindow();
        HDC hDesktopDC = GetDC(hDesktopWnd);
        HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
        HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, screenWidth, screenHeight);

        // Copy screen buffer memory into bitmap
        SelectObject(hCaptureDC, hCaptureBitmap);
        BitBlt(hCaptureDC, 0, 0, screenWidth, screenHeight, hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT);

        // Save bitmap to a file
        Gdiplus::Bitmap bitmap(hCaptureBitmap, NULL);
        CLSID clsid;
        getEncoderClsid(L"image/jpeg", &clsid);
        std::wstring wpath(path.begin(), path.end()); // Convert to wide string to prevent accidental chinese character conversion lmao
        bitmap.Save(wpath.c_str(), &clsid);

        // Cleanup
        ReleaseDC(hDesktopWnd, hDesktopDC);
        DeleteDC(hCaptureDC);
        DeleteObject(hCaptureBitmap);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
}

bool Screenshot::Process() const
{
    return true;
}

void Screenshot::compress()
{

}

void Screenshot::encrpyt()
{

}
