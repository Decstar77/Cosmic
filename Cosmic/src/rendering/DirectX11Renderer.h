#ifndef __DIRECTX11RENDERER_H__
#define __DIRECTX11RENDERER_H__

#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>

#include "Renderer.h"
#include "src/core/World.h"
#include "src/util/MemoryAreana.h"

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

#define DXRELEASE(object)  \
    if ((object))          \
    {                      \
        object->Release(); \
        object = nullptr;  \
    }

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

	class Texture
	{
	public:
		ID3D11Texture2D *texture = nullptr;
		ID3D11ShaderResourceView *view = nullptr;
		ID3D11SamplerState *sampler = nullptr;
	};

	class GraphicsContext;

	class DXMesh
	{
		friend class GraphicsContext;
		friend class DirectXImmediateRenderer;

	public:
		uint32 index = UINT32_MAX; // @TODO: Remove
		uint32 stride_bytes = UINT32_MAX;
		uint32 index_count = UINT32_MAX;
		uint32 vertex_size = UINT32_MAX;
		ID3D11Buffer *vertex_buffer = nullptr;
		ID3D11Buffer *index_buffer = nullptr;

	public:
		inline bool IsValid() { return vertex_buffer && index_buffer; }

		void Bind(GraphicsContext *gc) const;

	public:

		DXMesh();
		~DXMesh();
	};

	class DXShader
	{
		friend class GraphicsContext;
		friend class DirectXImmediateRenderer;

	public:
		ID3D11VertexShader *vs_shader = nullptr;
		ID3D11PixelShader *ps_shader = nullptr;
		ID3D11InputLayout *layout = nullptr;

	public:
		void Bind(GraphicsContext *gc) const;

	public:
		DXShader();
		~DXShader();
	};

	class DXTexture
	{
		friend class GraphicsContext;
		friend class DirectXImmediateRenderer;

	public:
		ID3D11Texture2D *texture = nullptr;
		ID3D11ShaderResourceView *view = nullptr;
		ID3D11SamplerState *sampler = nullptr;

	public:
		void Bind(GraphicsContext *gc, int32 register_index) const;

	public:
		DXTexture();
		~DXTexture();
	};

	class DXConstBuffer
	{

	};

	class GraphicsContext
	{
	public:
		bool InitializeDirectX(HWND window);
		void Destroy();

		void CreateMesh(MeshInstance *instance, EditableMesh *editable_mesh);
		void CreateShader(ShaderInstance *instance, EditableShader *editable_shader);
		void CreateTexture(TextureInstance *instance, EditableTexture *edtiable_texture);

		void Present();
		void ClearBuffer(const Vec4f &colour);

	public:
		ID3D11Device *device = nullptr;
		ID3D11DeviceContext *context = nullptr;

		IDXGISwapChain *swapchain = nullptr;
		ID3D11RenderTargetView *render_target = nullptr;
		ID3D11DepthStencilView *depth_target = nullptr;

		DirectXDebug debugger;
		HWND window;

		MemoryAreana<DXMesh> mesh_areana;
		MemoryAreana<DXShader> shader_areana;
		MemoryAreana<DXTexture> texture_areana;

		const int32 total_texture_resigters = 8;

	public:
		GraphicsContext();
		~GraphicsContext();

	private:


	private:
		int32 CreateMesh(real32 *vertex_data, uint32 vertex_size, uint32 vertex_stride_bytes,
			uint32 *index_data, uint32 index_count);
	};

	class WorldRenderer
	{
	public:
		void RenderWorld(World *world);
		void RenderMesh(const MeshInstance &mesh_instance, const Material &material, const Transform &transform);

		void SetMatrices(const Mat4f &view, const Mat4f &proj);

	public:
		WorldRenderer(GraphicsContext *graphics_context);
		WorldRenderer(const WorldRenderer &) = delete;
		WorldRenderer &operator=(const WorldRenderer &) = delete;
		~WorldRenderer();

	private:
		GraphicsContext *gc;

		ID3D11Buffer *uniform_buffer = nullptr;

		D3D11_VIEWPORT viewport;

		Mat4f view_matrix;
		Mat4f projection_matrix;

	};


	class DirectXDebugRenderer
	{
	public:
		ShaderInstance shader_instance;
		ID3D11Buffer *vertex_buffer = nullptr;
		ID3D11Buffer *const_buffer = nullptr;

	public:
		void SetMatrices(const Mat4f &view, const Mat4f &proj);
		void PushNewLine(const Vec3f &p0, const Vec3f &p1);// override;
		void PushNewLine(const Vec4f &p0, const Vec4f &p1);// override;
		void RenderAndFlush();

	public:
		DirectXDebugRenderer(GraphicsContext *graphics_context);
		DirectXDebugRenderer(const DirectXDebugRenderer &) = delete;
		DirectXDebugRenderer &operator=(const DirectXDebugRenderer &) = delete;
		~DirectXDebugRenderer();

	private:
		GraphicsContext *gc;

		int32 vertex_stride = 3;
		int32 vertex_count = 20000;
		int32 vertex_size_bytes = vertex_count * vertex_stride * sizeof(real32);
		int32 next_vertex_index = 0;
		real32 vertex_data[20000 * 3] = {};

		Mat4f view_matrix;
		Mat4f projection_matrix;

	private:
		void CreateVertexBuffer();
		void CreateConstBuffer();
		void PushLine(const Vec3f &a, const Vec3f &b);
		void PushLine(const Vec4f &a, const Vec4f &b);
	};

} // namespace cm

#endif // __DIRECTX11RENDERER_H__