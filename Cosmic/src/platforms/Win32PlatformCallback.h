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

#include <Windows.h>
#include "src/Cosmic.h"

namespace cm
{
	class Win32PlatformCallback
	{
	public:
		virtual	LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;

		static inline LRESULT Call(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			if (callback)
			{
				return callback->HandleMessage(hwnd, msg, wparam, lparam);
			}
			return false;
		}


	public:
		Win32PlatformCallback() { ASSERT(!callback, "Make more win32 callbacks"); callback = this; }
		virtual ~Win32PlatformCallback() { callback = nullptr; }

	private:
		inline static Win32PlatformCallback *callback = nullptr;

	};
}