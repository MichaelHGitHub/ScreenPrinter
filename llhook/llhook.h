
#pragma once 

#include <Windows.h>

#ifdef WIN64
const wchar_t szClientWindowClassName_[] = L"ScreenPrinterWindowClassName64";
#else
const wchar_t szClientWindowClassName_[] = L"ScreenPrinterWindowClassName";
#endif

// Define a message to notify client. 
// wParam : The virtual-key code of the nonsystem key
// lParam : reserved. 
const int MSG_KEY_DOWN = WM_USER + 1;
const int MSG_KEY_UP = WM_USER + 2; 

const int MSG_BUTTON_DOWN = WM_USER + 3;
const int MSG_BUTTON_UP = WM_USER + 4;

const int MSG_GESTURE = WM_USER + 5;


extern "C" bool _declspec (dllexport) InstallKeyboardHook(HWND hWindow);
extern "C" bool _declspec (dllexport) UnisntallKeyboardHook();

extern "C" bool _declspec (dllexport) InstallMouseHook(HWND hWindow);
extern "C" bool _declspec (dllexport) UninstallMouseHook();

extern "C" bool _declspec (dllexport) InstallGestureHook(HWND hWindow);
extern "C" bool _declspec (dllexport) UninstallGestureHook();