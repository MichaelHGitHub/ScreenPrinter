// llhook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <Windows.h>
#include <set>
#include "llhook.h"
#include <map>
#include <exception>

using namespace std;

extern HINSTANCE g_hModule;

namespace
{
	HHOOK keyboardHook_ = NULL;
	HWND hKeyboardClinet_ = NULL;

	HHOOK mouseHook_ = NULL;
	HWND hMouseClient_ = NULL;

	//HHOOK gestureHook_ = NULL;
	//HWND hGestureClient_ = NULL;

	map<DWORD, HHOOK> gestureHooks_; 

	LRESULT CALLBACK LowLevelKeyboardProc(_In_  int nCode,  _In_  WPARAM wParam,  _In_  LPARAM lParam);
	LRESULT CALLBACK LowLevelMouseProc(_In_  int nCode,  _In_  WPARAM wParam,  _In_  LPARAM lParam);
	LRESULT CALLBACK CallWndProc(_In_  int nCode, _In_  WPARAM wParam,  _In_  LPARAM lParam);



	bool IsInterestedKeyEvent(DWORD vkCode);
	bool IsInterestedMouseButton(DWORD vkCode);
	bool IsInterestedGesture(DWORD giGesture);

	std::set<DWORD> WantedKeys_;
	std::set<DWORD> WantedButtons_;
	std::set<DWORD> WantedGesture_;

}


extern "C" bool _declspec (dllexport) InstallKeyboardHook(HWND hWindow)
{
	bool result = false;

	if(::IsWindow(hWindow))
	{
		hKeyboardClinet_ = hWindow;

		WantedKeys_.insert(VK_PRIOR);

		keyboardHook_ = ::SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, ::GetModuleHandle(0), NULL);
		if(keyboardHook_ != NULL)
		{
			result = true;
		}

	}

	return result;
}


extern "C" bool _declspec (dllexport) UnisntallKeyboardHook()
{
	bool result = FALSE;

	if(keyboardHook_ != NULL)
	{
		result = UnhookWindowsHookEx(keyboardHook_) != FALSE;
	}

	return result;

}


extern "C" bool _declspec (dllexport) InstallMouseHook(HWND hWindow)
{
	bool result = false;

	if(::IsWindow(hWindow))
	{
		hMouseClient_ = hWindow;

		WantedButtons_.insert(VK_MBUTTON);

		mouseHook_ = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, ::GetModuleHandle(0), NULL);
		if(mouseHook_ != NULL)
		{
			result = true;
		}

	}

	return result;

}

extern "C" bool _declspec (dllexport) UninstallMouseHook()
{
	bool result = FALSE;

	if(mouseHook_ != NULL)
	{
		result = UnhookWindowsHookEx(mouseHook_) != FALSE;
	}

	return result;
}

extern "C" bool _declspec (dllexport) InstallGestureHook(HWND hTopWin)
{
	bool result = true;

	//HWND hDesktop = ::FindWindow(L"ScreepPrinterTestWindowName", NULL);

	//HWND hTopWin = ::GetTopWindow(NULL);

	try
	{
		DWORD threadId = ::GetWindowThreadProcessId(hTopWin, NULL );

		if(gestureHooks_.find(threadId) == gestureHooks_.end())
		{


			HHOOK gestureHook_ = ::SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, g_hModule, threadId);

			if(gestureHook_ != NULL)
			{
				gestureHooks_[threadId] = gestureHook_;
			}
			else
			{
				result = false;
			}
		}
	}
	catch(exception e)
	{
		result = false;
	}

	return result;

}


extern "C" bool _declspec (dllexport) UninstallGestureHook()
{
	bool result = true;

	try
	{
		for( map<DWORD, HHOOK>::iterator i = gestureHooks_.begin(); i != gestureHooks_.end(); i++ )
		{
			UnhookWindowsHookEx(i->second);
		}

		gestureHooks_.clear();

	}
	catch(exception e)
	{
		result = false;
	}

	return result;
}


namespace
{
	LRESULT CALLBACK LowLevelKeyboardProc(_In_  int nCode,  _In_  WPARAM wParam,  _In_  LPARAM lParam)
	{
		if(nCode == HC_ACTION)
		{
			KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;

			if(IsInterestedKeyEvent(pKeyInfo->vkCode))
			{
				if((pKeyInfo->flags & LLKHF_UP) == 0)
				{
					::PostMessage(hKeyboardClinet_, MSG_KEY_DOWN, (WPARAM)(pKeyInfo->vkCode), (LPARAM)0);
				}
				else
				{
					::PostMessage(hKeyboardClinet_, MSG_KEY_UP, (WPARAM)(pKeyInfo->vkCode), (LPARAM)0 );
				}

				return 1;
			}

		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	LRESULT CALLBACK LowLevelMouseProc(_In_  int nCode,  _In_  WPARAM wParam,  _In_  LPARAM lParam)
	{
		if(nCode == HC_ACTION)
		{
			MSLLHOOKSTRUCT * pMouseInfo = ( MSLLHOOKSTRUCT *)lParam;

			if(wParam == WM_MBUTTONDOWN)
			{
				::PostMessage(hMouseClient_, MSG_BUTTON_DOWN, (WPARAM)(pMouseInfo->mouseData), (LPARAM)0);
			}
			else if(wParam == WM_MBUTTONUP)
			{
				::PostMessage(hMouseClient_, MSG_BUTTON_UP, (WPARAM)(pMouseInfo->mouseData), (LPARAM)0 );
			}
			else
			{
				return CallNextHookEx(NULL, nCode, wParam, lParam);
			}

			return 1;

		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);

	}


	LRESULT CALLBACK CallWndProc(_In_  int nCode, _In_  WPARAM wParam,  _In_  LPARAM lParam)
	{
		if(nCode == HC_ACTION)
		{
			OutputDebugString( L"Hook a message" );

			 CWPSTRUCT* pMsgInfo = (CWPSTRUCT*)lParam;

			 if(pMsgInfo != NULL && (pMsgInfo->message == WM_GESTURE || pMsgInfo->message == WM_TOUCH))
			{
				OutputDebugString( L"Recieve a WM_GESTURE message" );

				GESTUREINFO gi;  
				ZeroMemory(&gi, sizeof(GESTUREINFO));
				gi.cbSize = sizeof(GESTUREINFO);

				BOOL bResult  = GetGestureInfo((HGESTUREINFO)(pMsgInfo->lParam), &gi);

				if(bResult)
				{
					if(gi.dwID == GID_TWOFINGERTAP)
					{
						HWND hWindow = ::FindWindow(szClientWindowClassName_, NULL);

						if(::IsWindow(hWindow))
						{
							BOOL pResult = ::PostMessage(hWindow, MSG_GESTURE, 0, 0);

							OutputDebugString( L"Post a MSG_GESTURE message" );

							if(pResult != TRUE)
							{
								wchar_t buffer[1024];

								::swprintf_s(buffer, 1024, L"PostMessage Last error is %d . ", ::GetLastError());

								OutputDebugString( buffer );
							}
						}
					}
				}
				else
				{
					wchar_t buffer[1024];

					::swprintf_s(buffer, 1024, L"Last error is %d . ", ::GetLastError());

					OutputDebugString( buffer );
				}
			 }
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}


	bool IsInterestedKeyEvent(DWORD vkCode)
	{
		return WantedKeys_.find(vkCode) != WantedKeys_.end();
	}

	bool IsInterestedMouseButton(DWORD vkCode)
	{
		return WantedButtons_.find(vkCode) != WantedButtons_.end();
	}

	bool IsInterestedGesture(DWORD giGesture)
	{
		return WantedGesture_.find(giGesture) != WantedGesture_.end();
	}

}





