#pragma once
#include "InputHandler.h"
#include "Time.h"

namespace Nimble 
{
	class Window 
	{
	public:
		HWND hwnd = NULL;
		HINSTANCE instance = NULL;
		eastl::unique_ptr<InputHandler> input;
		eastl::unique_ptr<Time> time;
		bool resized = false;

		Window(HINSTANCE hInstance_, int nCmdShow_);
		~Window();

		bool ProcessMessages();

	private:

		bool SetupWindow(int nCmdShow, WNDPROC windowProc);
		bool SetupConsole();
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}