
#pragma once

#include<Windows.h>
#include "MessageProxy.h"
#include "PrintScreenCommand.h"

class CommandServer : public MessageProxy
{
public:
	CommandServer(LPCWCHAR szWindowClass, LPCWCHAR szWindowTile);
	~CommandServer();

	bool IsValid() {return isValid;};
	LRESULT virtual HandleMessages(UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled);


private:

	LRESULT HandleGesture(UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled);

	void InitialAllThreads();

	bool isValid;

	PrintScreenCommand printCommand;

	HWND hLastTopWindow;


};