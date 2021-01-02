#ifndef __DIRECTX11RENDERER_H__
#define __DIRECTX11RENDERER_H__

#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>

#include "Renderer.h"
#include "src/serialization/FileParsers.h"

namespace cm
{
#define testingssss
#ifdef testingssss
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
#else
#define DXCHECK(call) (call);
#endif
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

	struct RenderingState
	{
		ID3D11Device *device;
		ID3D11DeviceContext *context;
		IDXGISwapChain *swapchain;
		ID3D11RenderTargetView *render_target;
		ID3D11DepthStencilView *depth_target;

		DirectXDebug debugger;
		HWND window;
	};

	class Texture
	{
	public:
		ID3D11Texture2D *texture = nullptr;
		ID3D11ShaderResourceView *view = nullptr;
		ID3D11SamplerState *sampler = nullptr;
	};

	class Pipeline
	{
	public:
		uint32 index;
		FileResult vertex_file;
		ID3D11PixelShader *ps_shader;
		FileResult pixel_file;
		ID3D11VertexShader *vs_shader;
		ID3D11InputLayout *layout = nullptr;

	public:
		Pipeline();
		~Pipeline();
	};

	class ShaderInstance
	{
	public:
		ShaderInstance(uint32 index) { this->index = index; }
		inline uint32 GetIndex() { return index; }

	private:
		uint32 index = UINT32_MAX;
	};

	class Mesh
	{
	public:
		uint32 index = UINT32_MAX;
		uint32 stride_bytes = UINT32_MAX;
		uint32 index_count = UINT32_MAX;
		uint32 vertex_size = UINT32_MAX;
		ID3D11Buffer *vertex_buffer = nullptr;
		ID3D11Buffer *index_buffer = nullptr;

	public:
		inline bool IsValid() { return vertex_buffer && index_buffer; }

	public:
		Mesh();
		~Mesh();
	};

	class RenderPrimitives
	{
		Mesh plane;
		Mesh cube;
		Mesh sphere;
		Mesh cyclinder;
		Mesh capsule;

	private:
		void Stub();
	};

	class DirectXState
	{
	public:
		static bool InitializeDirectX(HWND window);

	public:
		inline static ID3D11Device *device = nullptr;
		inline static ID3D11DeviceContext *context = nullptr;
		inline static IDXGISwapChain *swapchain = nullptr;
		inline static ID3D11RenderTargetView *render_target = nullptr;
		inline static ID3D11DepthStencilView *depth_target = nullptr;

		inline static DirectXDebug debugger;
		inline static HWND window;
	};

	class DirectXDebugRenderer : public DirectXState//, DebugRenderer
	{
	public:
		Pipeline pipeline;
		ID3D11Buffer *vertex_buffer = nullptr;
		ID3D11Buffer *const_buffer = nullptr;

	public:
		void SetMatrices(const Mat4f &view, const Mat4f &proj);
		void PushNewLine(const Vec3f &p0, const Vec3f &p1);// override;
		void PushNewLine(const Vec4f &p0, const Vec4f &p1);// override;
		void RenderAndFlush();

	public:
		DirectXDebugRenderer();
		DirectXDebugRenderer(const DirectXDebugRenderer &) = delete;
		DirectXDebugRenderer &operator=(const DirectXDebugRenderer &) = delete;
		~DirectXDebugRenderer();

	private:
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

	class DirectXImmediateRenderer : public DirectXState
	{
	public:
		inline static DirectXImmediateRenderer *GetInstance();
		void LOGGPUCards();

		void SetMatrices(const Mat4f &view, const Mat4f &proj);
		void SetTexture(Texture texture);

		MeshInstance CreateMesh(const String &file_name, const real32 &scale = 1.0f);
		MeshInstance CreateMesh(EditableMesh &mesh);
		MeshInstance CreateMesh(real32 *vertex_data, uint32 vertex_size, uint32 vertex_stride_bytes,
			uint32 *index_data, uint32 index_count);
		void FreeMesh(MeshInstance instance);

		Pipeline CreatePipline(const String &vertex_dir, const String &pixel_dir);

		Texture CreateTexture(const String &file_name);
		Texture CreateTexture(uint32 width, uint32 height, uint32 num_channels, uint8 *data);
		void FreeTexture(Texture *texture);

		void ClearBuffer(const Vec4f &colour);
		void RenderMesh(const MeshInstance &mesh_instance, const Transform &transform);
		void RenderMesh(const MeshInstance &mesh_instance, const Mat4f &transform_matrix);
		void RenderQuad(const MeshInstance &mesh_instance, const Mat4f &transform);
		void EndFrame();

	public:
		DirectXImmediateRenderer(HWND window);
		DirectXImmediateRenderer(const DirectXImmediateRenderer &) = delete;
		DirectXImmediateRenderer &operator=(const DirectXImmediateRenderer &) = delete;

		~DirectXImmediateRenderer();

		Pipeline shader;

	private:
		inline static DirectXImmediateRenderer *instance = nullptr;
		ID3D11Buffer *const_buffer = nullptr;
		Mat4f view_matrix;
		Mat4f projection_matrix;

		const uint32 total_mesh_count = 1000;
		std::vector<Mesh> meshes;
		std::stack<uint32> mesh_free_list;

		const uint32 total_shader_count = 1000;
		std::vector<Pipeline> shaders;
		std::stack<uint32> shader_free_list;

		void CreateDevice();
		void CreateArrays();
	};

	class MeshBuilder
	{
	public:
		std::vector<real32> vertices;
		std::vector<uint32> indices;
		uint32 stride = sizeof(real32) * 3 + sizeof(real32) * 3 + sizeof(uint32) * 2; // @NOTE: Temp

	public:
		MeshBuilder(const String &path);
		~MeshBuilder();
	};
} // namespace cm

#endif // __DIRECTX11RENDERER_H__