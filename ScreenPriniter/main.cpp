#include "stdafx.h"

#include <windows.h>
#include <iostream>

#include "llhook.h"
#include "CommandServer.h"

namespace
{
	wchar_t szWindowTile[] = L"ScreenPrinterWindow";
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
	CommandServer commandServer(szClientWindowClassName_, szWindowTile);

	if(commandServer.IsValid())
	{
		MSG message = {0};
		while(::GetMessage(&message, NULL, 0, 0) != 0)
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}

	return 0;
}
