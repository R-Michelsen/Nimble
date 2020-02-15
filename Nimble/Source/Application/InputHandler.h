#pragma once
#include <EASTL/array.h>

#include "InputTypes.h"

namespace Nimble 
{
	class InputHandler 
	{
	public:
		POINT mousePos{};
		MouseDelta mouseDelta{};
		MouseButtonStatus mouseButtonActive{};
		MouseButtonStatus mouseButtonPressed{};
		MouseDragStatus mouseDrag{};

		eastl::array<unsigned char, 0xFF> keyState;
		eastl::array<unsigned char, 0xFF> keyPressed;
		KeyboardControlKeys controlKeysPressed{};

		void NewFrame();
		void ProcessRawInput(RAWINPUT* rawInput, HWND hwnd);
	};
}