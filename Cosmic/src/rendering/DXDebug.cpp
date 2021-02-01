#include "DXDebug.h"

namespace cm
{
	DirectXDebug::DirectXDebug()
	{
		HRESULT dxresult;
		typedef HRESULT(WINAPI * DXGIGetDebugInterface)(REFIID, void **);

		HMODULE mod_debug = LoadLibraryExA("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (mod_debug)
		{
			DXGIGetDebugInterface debug_fnc = reinterpret_cast<DXGIGetDebugInterface>(reinterpret_cast<void *>(GetProcAddress(mod_debug, "DXGIGetDebugInterface")));
			if (debug_fnc)
			{
				dxresult = debug_fnc(__uuidof(IDXGIInfoQueue), (void **)&info_queue);
			}
			else
			{
				Platform::ErrorBox("Could not find DX debug fnc in dll");
			}
		}
		else
		{
			Platform::ErrorBox("Could not find DX debug dll");
		}
	}

	DirectXDebug::~DirectXDebug()
	{
		if (info_queue)
		{
			info_queue->Release();
		}
	}

	void DirectXDebug::Set()
	{
		next = info_queue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	}

	std::vector<String> DirectXDebug::GetMessages() const
	{
		uint64 end = info_queue->GetNumStoredMessages(DXGI_DEBUG_ALL);
		std::vector<String> messages;
		// @NOTE: I think end - next is the current size needed for the array;
		// @TODO: MAke this with out array lol
		for (uint64 i = next; i < end; i++)
		{
			SIZE_T messageLength;
			info_queue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);

			auto bytes = std::make_unique<byte[]>(messageLength);
			auto message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE *>(bytes.get());

			info_queue->GetMessage(DXGI_DEBUG_ALL, i, message, &messageLength);
			messages.emplace_back(message->pDescription);
		}

		return messages;
	}
}