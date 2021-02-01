#pragma once
#include "src/Cosmic.h"

#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>

#define GRAPHICS_DEBUG 1 // @TODO

#include "DXDebug.h"


namespace cm
{
#define GETDEUBBGER() DirectXDebug &debugger = gc->debugger;

#define DXCHECK(call)                                                                                                   \
    {                                                                                                                   \
        debugger.Set();                                                                                                 \
        HRESULT dxresult = (call);                                                                                      \
        if (FAILED(dxresult))                                                                                           \
        {                                                                                                               \
            char *output = nullptr;                                                                                     \
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, \
                           NULL, dxresult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&output, 0, NULL);         \
            if (output)                                                                                                 \
            {                                                                                                           \
                std::vector<String> messages = debugger.GetMessages();                                                        \
                for (int32 i = 0; i < messages.size(); i++)                                                             \
                {                                                                                                       \
                    LOG(messages[i])                                                                                    \
                }                                                                                                       \
                Platform::ErrorBox(output);                                                                                \
            }                                                                                                           \
        }                                                                                                               \
    }

#define DXINFO(call)                                         \
    {                                                        \
        debugger.Set();                                      \
        (call);                                              \
        std::vector<String> messages = debugger.GetMessages();     \
        if (messages.size() > 0)                             \
        {                                                    \
            StringStream ss;                                 \
            for (int32 i = 0; i < messages.size(); i++)      \
            {                                                \
                LOG(messages[i]);                            \
                ss << messages[i] << '\n'                    \
                   << "====================================" \
                   << "\n";                                  \
            }                                                \
            Platform::ErrorBox(ss.str());                       \
        }                                                    \
    }


	//@NOTE: This is not a debug thing, it will be used in release !!!
#define DXRELEASE(object)  \
    if ((object))          \
    {                      \
        object->Release(); \
        object = nullptr;  \
    }


	class GraphicsContext;

	enum class ShaderStage // @TODO: Put in shader too, for nicesness
	{
		INVALID = 0,
		VERTEX_SHADER,
		PIXEL_SHADER
	};

}