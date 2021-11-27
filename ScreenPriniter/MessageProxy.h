/************************************************************\
* Module Name:  MessageProxy.h
* Project:      CppWindowsService
* Copyright (c) Microsoft Corporation.
* 
* Implement Keyboard hook logic:
*   
\***************************************************************************/

#pragma once 

#include <Windows.h>
#include <string>

const UINT KEYBOARD_HOOK_EVENT = WM_USER + 1;

class MessageProxy
{

public :

    MessageProxy(LPCWCHAR szWindowClass, LPCWCHAR szWindowTile);
    ~MessageProxy();

    //HRESULT Run();
    //void Stop();

    HWND GetWindowHandle();

	LRESULT virtual HandleMessages(UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled);

	void virtual OnDestroy();
	void virtual OnCreate();

protected:

protected:
		int shellHookMessageID_;

private:

    BOOL InitInstance(LPCWCHAR szWindowClass, LPCWCHAR szWindowTile);

    HWND m_hWnd;
	std::wstring szClassName;



    //bool toStop;

};