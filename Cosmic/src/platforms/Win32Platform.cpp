#include "Win32Platform.h"

namespace cm
{
#if 1
	void Window::ErrorBox_(const String &msg)
	{
		MessageBoxA(NULL, msg.c_str(), "ERROR", MB_OK | MB_ICONERROR);
	}

	void Window::WarningBox_(const String &msg)
	{
		MessageBoxA(NULL, msg.c_str(), "WARNING", MB_OK | MB_ICONERROR);
	}

	bool Window::QuestionBox_(const String &msg)
	{
		int res = MessageBoxA(NULL, msg.c_str(), "Don Que ?", MB_YESNO | MB_ICONEXCLAMATION);

		return (res == IDYES);
	}

	bool Window::IsOpen()
	{
		return (bool)window && window_running;
	}

	HWND Window::GetHandle()
	{
		return window;
	}

	void Window::EnableRawInput()
	{
		if (raw_input_initialized)
		{
			MouseInput::EnableRawInput();
		}
		else
		{
			ErrorBox("Raw input has not been initializd!!");
		}
	}

	void Window::EnableMouse()
	{
		ShowCursor(TRUE);
	}

	void Window::DisableMouse()
	{
		ShowCursor(FALSE);
	}

	inline int32 Window::GetClientWidth_()
	{
		return (int32)client_dimensions.x;
	}

	inline int32 Window::GetClientHeight_()
	{
		return (int32)client_dimensions.y;
	}

	inline int32 Window::GetWindowWidth()
	{
		return (int32)window_width;
	}

	inline int32 Window::GetWindowHeight()
	{
		return (int32)window_height;
	}

	bool Window::PollMessages()
	{
		if (window_running)
		{
			KeyInput::SetLastFromCurrent();
			MouseInput::SetLastFromCurrent();
			ProcessMouseInput();
			ProcessKeyboardInput();

			MSG msg = {};
			while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_CLOSE)
				{
					window_running = false;
				}

				if (msg.message == WM_LBUTTONDOWN)
				{
					MouseInput::SetMouseButtonState(MouseCode::LEFT_MOUSE_BUTTON, true);
				}

				if (msg.message == WM_LBUTTONUP)
				{
					MouseInput::SetMouseButtonState(MouseCode::LEFT_MOUSE_BUTTON, false);
				}

				if (msg.message == WM_RBUTTONDOWN)
				{
					MouseInput::SetMouseButtonState(MouseCode::RIGHT_MOUSE_BUTTON, true);
				}

				if (msg.message == WM_RBUTTONUP)
				{
					MouseInput::SetMouseButtonState(MouseCode::RIGHT_MOUSE_BUTTON, false);
				}

				TranslateMessage(&msg);
				DispatchMessageA(&msg);
			}
		}

		return window_running;
	}

	Window::Window(const String &window_title, const Vec2f &client_dimensions, HINSTANCE hInstance) : window_title(window_title)
	{
		CreateConsole();

		RECT client_rect;
		client_rect.left = 0;
		client_rect.right = (LONG)client_dimensions.x;
		client_rect.top = 0;
		client_rect.bottom = (LONG)client_dimensions.y;

		DWORD window_style = WS_VISIBLE | (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME);
		AdjustWindowRect(&client_rect, window_style, FALSE);

		WNDCLASSEXA window_class = {};
		window_class.cbSize = sizeof(window_class);
		window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc = &HandleMsgSetup;
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = 0;
		window_class.hInstance = hInstance;
		window_class.hIcon = nullptr;
		window_class.hIconSm = nullptr;
		window_class.hCursor = nullptr;
		window_class.hbrBackground = nullptr;
		window_class.lpszClassName = window_title.c_str();

		RegisterClassExA(&window_class);

		window = CreateWindowExA(
			NULL, window_class.lpszClassName, window_title.c_str(), window_style,
			CW_USEDEFAULT, CW_USEDEFAULT, client_rect.right - client_rect.left, client_rect.bottom - client_rect.top,
			NULL, NULL, hInstance, this);

		GetClientRect(window, &client_rect);
		this->client_dimensions.x = static_cast<real32>(client_rect.right - client_rect.left);
		this->client_dimensions.y = static_cast<real32>(client_rect.bottom - client_rect.top);

		if (window)
		{
			InitializeRawInput();

			HANDLE icon = LoadImageA(0, "res/cosmic_icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
			if (icon)
			{
				SendMessage(window, WM_SETICON, ICON_SMALL, (LPARAM)icon);
				SendMessage(window, WM_SETICON, ICON_BIG, (LPARAM)icon);

				SendMessage(GetWindow(window, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)icon);
				SendMessage(GetWindow(window, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)icon);
			}

			RECT window_rect;
			GetWindowRect(window, &window_rect);
			window_width = window_rect.right - window_rect.left;
			window_height = window_rect.bottom - window_rect.top;

			window_running = true;

			MouseInput::EnableMouse();
			if (!PollMessages())
			{
				ASSERT(0, "INVALID");
			}
		}
	}

	Window::~Window()
	{
	}

	void Window::CreateConsole()
	{
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE *stream;
		freopen_s(&stream, "CONOUT$", "w+", stdout);
		freopen_s(&stream, "CONOUT$", "w+", stderr);

		HWND console = GetConsoleWindow();
		SetWindowPos(console, 0, -1920 / 2 - 500, 1080 / 2 - 250, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	void Window::InitializeRawInput()
	{
		if (!raw_input_initialized)
		{
			{
				RAWINPUTDEVICE rid = {};
				rid.usUsagePage = 0x1; // @NOTE: Mouse
				rid.usUsage = 0x02;
				rid.dwFlags = 0;
				rid.hwndTarget = NULL; // @NOTE: I think this means listen to events outside of window

				if (RegisterRawInputDevices(&rid, 1, sizeof(rid)))
				{
					raw_input_initialized = true;
				}
				else
				{
					WarningBox("Could not init raw input");
				}
			}
			{
				RAWINPUTDEVICE rid = {};
				rid.usUsagePage = 0x1; // @NOTE: Mouse
				rid.usUsage = 0x06;
				rid.dwFlags = 0;
				rid.hwndTarget = NULL; // @NOTE: I think this means listen to events outside of window

				if (RegisterRawInputDevices(&rid, 1, sizeof(rid)))
				{
					raw_input_initialized = true;
				}
				else
				{
					WarningBox("Could not init raw input");
				}
			}
		}
	}

	void Window::ProcessRawInput(LPARAM lparam)
	{
		if (raw_input_initialized && MouseInput::IsRawInput())
		{
			uint32 size;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

			if (size > 0)
			{
				std::vector<uint8> data(size);
				uint32 read = GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, data.data(), &size, sizeof(RAWINPUTHEADER));

				if (read == size)
				{
					RAWINPUT *input = reinterpret_cast<RAWINPUT *>(data.data());
					if (input->header.dwType == RIM_TYPEMOUSE)
					{
						real32 x = static_cast<real32>(input->data.mouse.lLastX);
						real32 y = static_cast<real32>(input->data.mouse.lLastY);

						MouseInput::delta += Vec2f(x, -y);
					}
					else if (input->header.dwType == RIM_TYPEKEYBOARD)
					{
						if (input->data.keyboard.Flags == RI_KEY_MAKE)
						{
							KeyInput::SetKeyState(input->data.keyboard.VKey, true);
						}
						else if (input->data.keyboard.Flags == RI_KEY_BREAK)
						{
							KeyInput::SetKeyState(input->data.keyboard.VKey, false);
						}
					}
				}
			}
		}
	}

	void Window::ProcessKeyboardInput()
	{
		std::array<uint8, COSMIC_KEY_COUNT> input_keys = {};

		if (GetKeyboardState(input_keys.data()))
		{
			for (uint32 i = 0; i < COSMIC_KEY_COUNT; i++)
			{
				KeyInput::SetKeyState(i, (input_keys[i] & 0x80) ? true : false);
			}
		}
	}

	void Window::ProcessMouseInput()
	{
		MouseInput::SetMouseDelta(0);

		if (MouseInput::IsEnabled())
		{
			if (!mouse_shown)
			{
				if (ShowCursor(TRUE) >= 0)
				{
					mouse_shown = true;
				}
			}

			POINT current_point = {};
			if (GetCursorPos(&current_point))
			{
				if (ScreenToClient(window, &current_point))
				{
					MouseInput::SetLastPosition(MouseInput::GetCurrentPosition());
					MouseInput::SetCurrentPosition(Vec2f(static_cast<real32>(current_point.x), static_cast<real32>(current_point.y)));
					MouseInput::SetMouseDelta(MouseInput::GetCurrentPosition() - MouseInput::GetLastPosition());
				}
			}
		}
		else
		{
			if (mouse_shown)
			{
				if (ShowCursor(FALSE) < 0)
				{
					mouse_shown = false;
				}
			}


			if (MouseInput::IsRawInput())
			{
				POINT center_point = {};
				if (ClientToScreen(window, &center_point))
				{
					center_point.x += static_cast<LONG>(client_dimensions.x) / 2;
					center_point.y += static_cast<LONG>(client_dimensions.y) / 2;

					SetCursorPos(center_point.x, center_point.y);

					MouseInput::SetCurrentPosition(0);
					MouseInput::SetLastPosition(0);
				}
			}
			else
			{
				ASSERT(0, "Cannot have disabled mouse without raw input");
			}
		}
#endif 
	}

	LRESULT Window::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		LRESULT result = Win32PlatformCallback::Call(hwnd, msg, wparam, lparam);

		if (!result)
		{
			if (this != nullptr)
			{
				switch (msg)
				{

					//case WM_SETFOCUS:
					//{
					//	window_focused = true;
					//	result = DefWindowProcA(hwnd, msg, wparam, lparam);
					//}
					//break;

					//case WM_KILLFOCUS:
					//{
					//	window_focused = false;
					//	result = DefWindowProcA(hwnd, msg, wparam, lparam);
					//}
					//break;

				case WM_SIZE:
				{
				}
				break;

				case WM_DESTROY:
				{
					window_running = false;
					PostQuitMessage(0);
				}
				break;

				case WM_CLOSE:
				{
					window_running = false;
					PostQuitMessage(0);
				}
				break;

				case WM_ACTIVATEAPP:
				{
				}
				break;

				case WM_MOUSEMOVE:
				{
					//real32 x = LOWORD(lparam);
					//real32 y = HIWORD(lparam);

					//MouseInput::SetLastPosition(MouseInput::GetCurrentPosition());
					//MouseInput::SetCurrentPosition(Vec2f(static_cast<real32>(x), static_cast<real32>(y)));
					//MouseInput::SetMouseDelta(MouseInput::GetCurrentPosition() - MouseInput::GetLastPosition());

					//POINT center_point = {};
					//if (ClientToScreen(window, &center_point))
					//{
					//	center_point.x += static_cast<LONG>(client_dimensions.x) / 2;
					//	center_point.y += static_cast<LONG>(client_dimensions.y) / 2;

					//	SetCursorPos(center_point.x, center_point.y);
					//}

					//InputCallbacks::PostMouseCallbacks();
				}
				break;

				case WM_INPUT:
				{
					ProcessRawInput(lparam);
					result = DefWindowProcA(hwnd, msg, wparam, lparam);
				}
				break;

				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
				case WM_KEYUP:
				case WM_KEYDOWN:
				{
					int32 code = (int32)wparam;
					bool was_down = ((lparam & (1 << 30)) != 0);
					bool is_down = ((lparam & (1 << 31)) == 0);


					if (code == VK_SPACE)
					{
					}
					else if (code == VK_ESCAPE)
					{
						window_running = false;
					}

					result = DefWindowProcA(hwnd, msg, wparam, lparam);
				}
				break;

				default:
				{
					result = DefWindowProcA(hwnd, msg, wparam, lparam);
				}
				}
			}
			else
			{
				result = DefWindowProcA(hwnd, msg, wparam, lparam);
			}
		}
		return result;
	}

	LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// @NOTE: Use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
		if (msg == WM_NCCREATE)
		{
			// @NOTE: Extract ptr to window class from creation data
			const CREATESTRUCTW *const pCreate = reinterpret_cast<CREATESTRUCTW *>(lParam);
			Window *const pWnd = static_cast<Window *>(pCreate->lpCreateParams);

			// @NOTE: Set WinAPI-managed user data to store ptr to window instance
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

			// @NOTE: Set message proc to normal (non-setup) handler now that setup is finished
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

			// @NOTE: Forward message to window instance handler
			return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
		}
		// if we get a message before the WM_NCCREATE message, handle with default handler
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// retrieve ptr to window instance
		Window *const pWnd = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		// forward message to window instance handler
		return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
	}


	bool Window::IsWindowFocused_()
	{
		return window_focused;
	}
} // namespace cm