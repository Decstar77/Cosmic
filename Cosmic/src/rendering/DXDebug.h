#pragma once

#include "src/Cosmic.h"

#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>

namespace cm
{
	class DirectXDebug
	{
	public:
		DirectXDebug();
		DirectXDebug(const DirectXDebug &) = delete;
		DirectXDebug &operator=(const DirectXDebug &) = delete;
		~DirectXDebug();

		void Set();
		std::vector<String> GetMessages() const;

	private:
		uint64 next = 0u;
		struct IDXGIInfoQueue *info_queue = nullptr;
	};
}