#include <NimblePCH.h>

#include "Window.h"

#include "Logging.h"

namespace Nimble {
	Window::Window(HINSTANCE instance_, int nCmdShow) : instance(instance_)
	{
		// No Windows bitmap-stretching
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

		if (!SetupWindow(nCmdShow, WndProc)) 
		{
			NIMBLE_LOG_ERROR("Failed to setup win32 Window\n");
			return;
		}

		if (!RegisterRawInputDevices(&RAW_INPUT_DEVICES[0], 2, sizeof(RAWINPUTDEVICE))) 
		{
			NIMBLE_LOG_ERROR("Failed to register input devices\n");
			return;
		}
		input = eastl::make_unique<InputHandler>();
		time = eastl::make_unique<Time>();

		if (!SetupConsole()) 
		{
			NIMBLE_LOG_ERROR("Failed to setup the console\n");
			return;
		}
	}

	Window::~Window() 
	{
		DestroyWindow(hwnd);
		UnregisterClassW(L"Nimble_Class", instance);
	}

	bool Window::ProcessMessages() 
	{
		input->NewFrame();
		time->UpdateTimer();

		MSG msg;
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
			if (msg.message == WM_QUIT) return false;
		}
		return true;
	}

	bool Window::SetupWindow(int nCmdShow, WNDPROC windowProc) 
	{
		// Could make these parameters but fine as const for now
		auto windowClassName = L"Nimble_Class";
		auto windowTitle = L"Nimble Engine";

		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = windowProc;
		windowClass.hInstance = instance;
		windowClass.lpszClassName = windowClassName;
		windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);

		if (!RegisterClassExW(&windowClass))
		{
			return false;
		}

		hwnd = CreateWindowExW(
			0,
			windowClassName,
			windowTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			2560, // Resolution width
			1440, // Resolution height
			NULL,
			NULL,
			instance,
			this
		);

		if (hwnd == NULL) return false;
		ShowWindow(hwnd, nCmdShow);

		return true;
	}

	bool Window::SetupConsole() 
	{
		if (!AllocConsole()) return false;
		if (!freopen("CONIN$", "r", stdin)) return false;
		if (!freopen("CONOUT$", "w", stdout)) return false;
		if (!freopen("CONOUT$", "w", stderr)) return false;
		return true;
	}

	LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
	{
		Window* window;
		if (uMsg == WM_NCCREATE) 
		{
			window = static_cast<Window*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
			SetLastError(0);
			if (!SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window))) 
			{
				if (GetLastError() != 0) return FALSE;
			}
		}
		else
		{
			window = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		}

		PAINTSTRUCT ps;
		HDC hdc;

		switch (uMsg)
		{
		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_SIZE:
		{
			window->resized = true;
			break;
		}
		case WM_DESTROY: 
		{
			PostQuitMessage(0);
			break;
		}
		case WM_INPUT: 
		{
			uint32_t inputSize = 0;
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &inputSize, sizeof(RAWINPUTHEADER));
			LPBYTE rawInput = new BYTE[inputSize];
			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, rawInput, &inputSize, sizeof(RAWINPUTHEADER)) != inputSize) 
			{
				NIMBLE_LOG_ERROR("GetRawInputData didn't return correct size!\n");
			}
			else 
			{
				window->input->ProcessRawInput((RAWINPUT*)rawInput, window->hwnd);
				delete[] rawInput;
			}
			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
			window->input->mousePos = POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}