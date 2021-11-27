
#include "PrintScreenCommand.h"

PrintScreenCommand::PrintScreenCommand()
{

}

PrintScreenCommand::~PrintScreenCommand()
{

}

void PrintScreenCommand::Execute()
{
	KEYBDINPUT keyInputDown = {0};
	KEYBDINPUT keyInputUp = {0};

	INPUT keys[2];

	memset( &keys[0], 0, sizeof(keys[0]) );
	memset( &keys[1], 0, sizeof(keys[1]) );

	keyInputDown.wVk = VK_SNAPSHOT;
	keyInputDown.wScan = ::MapVirtualKey(VK_SNAPSHOT, 0);

	keyInputUp.wVk = VK_SNAPSHOT;
	keyInputUp.wScan = ::MapVirtualKey(VK_SNAPSHOT, 0);
	keyInputUp.dwFlags = KEYEVENTF_KEYUP;


	keys[0].type = INPUT_KEYBOARD;
	keys[0].ki = keyInputDown;

	keys[1].type = INPUT_KEYBOARD;
	keys[1].ki = keyInputUp;


	::SendInput(2, keys, sizeof(INPUT));

}