

#include "stdafx.h"

#include <Windows.h>
#include "CommandServer.h"
#include "llhook.h"


namespace
{
	BOOL CALLBACK EnumWindowsProc(_In_  HWND hwnd, _In_  LPARAM lParam);

	BOOL CALLBACK EnumChildWindowProc(_In_ HWND hwnd, _In_ LPARAM lParam);

}

CommandServer::CommandServer(LPCWCHAR szWindowClass, LPCWCHAR szWindowTile):
	MessageProxy(szWindowClass, szWindowTile),
	isValid(false),
	hLastTopWindow(NULL)
{
	if(::IsWindow(GetWindowHandle()))
	{
		isValid = InstallKeyboardHook(GetWindowHandle());
		isValid = InstallMouseHook(GetWindowHandle());
		//isValid = InstallGestureHook();

		//InitialAllThreads();
		//isValid = true;

	}
}


CommandServer::~CommandServer()
{
	//UnisntallKeyboardHook();
	//UninstallMouseHook();
	UninstallGestureHook();


}

void CommandServer::InitialAllThreads()
{
	::EnumWindows( EnumWindowsProc, NULL);
}

LRESULT CommandServer::HandleMessages(UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled)
{
	LRESULT result = FALSE;

	if(msg == shellHookMessageID_)
	{
		if(wParam == HSHELL_WINDOWACTIVATED || wParam == HSHELL_RUDEAPPACTIVATED)
		{
			//HWND hTopWin = ::GetTopWindow(NULL);

			if(hLastTopWindow != (HWND)lParam)
			{
				//UninstallGestureHook();
				hLastTopWindow = (HWND)lParam;

				if(::IsWindowVisible(hLastTopWindow))
				{
					InstallGestureHook(hLastTopWindow);
				}

				::EnumChildWindows(hLastTopWindow, EnumChildWindowProc, NULL);
			}
		}

		isHandled = true;
	}
	else
	{

		switch(msg)
		{
		case MSG_KEY_DOWN:
		case MSG_BUTTON_DOWN:

			printCommand.Execute();
			isHandled = true;
			break;

		case MSG_KEY_UP:
		case MSG_BUTTON_UP:

			isHandled = true;
			break;

		case MSG_GESTURE:
			printCommand.Execute();

			OutputDebugString(L"Receive MSG_GESTURE message");

			isHandled = true;

			//	result = HandleGesture(msg, wParam, lParam, isHandled);

			break;

		default:

			break;

		}

	}

	return result;
}


LRESULT CommandServer::HandleGesture(UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled)
{
	LRESULT result = 1;

	GESTUREINFO gi;  

	ZeroMemory(&gi, sizeof(GESTUREINFO));

	gi.cbSize = sizeof(GESTUREINFO);

	BOOL bResult  = GetGestureInfo((HGESTUREINFO)lParam, &gi);

	if (bResult)
	{
		// now interpret the gesture
		switch (gi.dwID)
		{
		case GID_PRESSANDTAP:
			printCommand.Execute();

			result = 0;
			isHandled = true;
			break;
		default:
			// A gesture was not recognized
			break;
		}
	}
	else
	{
		DWORD dwErr = GetLastError();
		if (dwErr > 0)
		{
			//MessageBoxW(hWnd, L"Error!", L"Could not retrieve a GESTUREINFO structure.", MB_OK);
		}
	}

	return result;

}
namespace
{
	BOOL CALLBACK EnumWindowsProc(_In_  HWND hwnd, _In_  LPARAM lParam)
	{
		if(::IsWindowVisible(hwnd))
		{
			InstallGestureHook(hwnd);
		}
		else
		{
			::EnumChildWindows(hwnd,EnumChildWindowProc, lParam);
		}

		return TRUE;
	}

	BOOL CALLBACK EnumChildWindowProc(_In_  HWND hwnd, _In_  LPARAM lParam)
	{
		BOOL result = TRUE;

		if(::IsWindowVisible(hwnd))
		{
			if(InstallGestureHook(hwnd))
			{
				//result = FALSE;
			}
		}

		return result;
	}

}
