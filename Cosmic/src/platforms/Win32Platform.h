// target Windows 7 or later
#pragma once
#define _WIN32_WINNT 0x0601

#ifndef FULL_WINTARD
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define NOMINMAX
#define STRICT
#endif

#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"
#include "src/input/MouseInput.h"
#include "src/input/KeyInput.h"
#include "src/input/InputCallbacks.h"
#include "Win32PlatformCallback.h"

#include <Windows.h>


#include <fstream>
#include <filesystem>

namespace cm
{
#if 1



	class Window : public Platform
	{
	public:
		void ErrorBox_(const String &msg) override;
		void WarningBox_(const String &msg) override;
		bool QuestionBox_(const String &msg) override;

		int32 GetClientWidth_() override;
		int32 GetClientHeight_() override;

	public:
		bool IsOpen();
		bool PollMessages();
		HWND GetHandle();

		void EnableRawInput();
		void EnableMouse();
		void DisableMouse();

		int32 GetWindowWidth();
		int32 GetWindowHeight();

	public:
		Window(const String &window_title, const Vec2f &client_dimensions, HINSTANCE hInstance);
		Window(const Window &) = delete;
		Window &operator=(const Window &) = delete;
		~Window();

	private:
		HWND window;
		HINSTANCE instance;

		bool window_running;
		bool window_focused;
		String window_title;
		Vec2f window_center;
		Vec2f client_dimensions;

		int32 window_width;
		int32 window_height;

		bool mouse_shown = true;
		bool raw_input_initialized = false;

	private:
		void CreateConsole();
		void InitializeRawInput();
		void ProcessRawInput(LPARAM lparam);
		void ProcessKeyboardInput();
		void ProcessMouseInput();
		LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	protected:
		virtual bool IsWindowFocused_() override;

	};
#endif
} // namespace cm