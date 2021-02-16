#ifndef __DIRECTX11RENDERER_H__
#define __DIRECTX11RENDERER_H__

#include "DirectX11.h"

#include "DXMesh.h"
#include "DXShader.h"
#include "DXTexture.h"
#include "DXConstantBuffer.h"
#include "DXRenderTarget.h"

#include "Renderer.h"
#include "src/core/World.h"
#include "src/core/Clock.h"
#include "src/util/MemoryAreana.h"

namespace cm
{
	class GraphicsContext;

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

	class GraphicsContext
	{
		friend class WorldRenderer;

	public:
		bool InitializeDirectX(HWND window);
		void Destroy();

		void CreateMesh(MeshInstance *instance, EditableMesh *editable_mesh);
		void CreateShader(ShaderInstance *instance, EditableShader *editable_shader);
		void CreateTexture(TextureInstance *instance, EditableTexture *edtiable_texture);


		void Present();
		void ClearBuffer(const Vec4f &colour);

		void LogGPUS();

		inline bool IsMSAAEnabled() { return msaa_sample_count > 1; }

	public:
		ID3D11Device *device = nullptr;
		ID3D11DeviceContext *context = nullptr;

		ID3D11RasterizerState *rs_standard_state = nullptr;
		ID3D11RasterizerState *rs_shadow_state = nullptr;

		IDXGISwapChain *swapchain = nullptr;

		DXRenderTarget swapchain_render_target;

		ID3D11RenderTargetView *render_target = nullptr;
		ID3D11DepthStencilView *depth_target = nullptr;

		DirectXDebug debugger;
		HWND window;

		MemoryAreana<DXMesh> mesh_areana;
		MemoryAreana<DXShader> shader_areana;
		MemoryAreana<DXTexture> texture_areana;

		const int32 total_texture_resigters = 8;

		int32 msaa_sample_count = 1;
		int32 quality_level = 0;

	public:
		GraphicsContext();
		~GraphicsContext();

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

		ShaderInstance post_processing_shader;
		ShaderInstance depth_writer_shader;

		DirectXDebugRenderer *debug_renderer = nullptr;

	public:
		WorldRenderer(GraphicsContext *graphics_context);
		WorldRenderer(const WorldRenderer &) = delete;
		WorldRenderer &operator=(const WorldRenderer &) = delete;
		~WorldRenderer();

	private:
		void UploadLightingInfo(World *world);
		void ShadowPass(const std::vector<Entity*> &entities, DirectionalLight *light);
		void RenderQuad();

	private:
		GraphicsContext *gc;

		ID3D11Buffer *uniform_buffer = nullptr;

		DXConstBuffer ps_lighting_buffer;
		DXConstBuffer vs_lighting_buffer;

		ID3D11RenderTargetView *current_render_target;
		ID3D11SamplerState *shadow_sampler;

		DXRenderTarget offscreen_render_target;
		DXRenderTarget shadow_render_target;

		int32 screen_space_quad_index;

		D3D11_VIEWPORT viewport;

		Mat4f view_matrix;
		Mat4f projection_matrix;
	};




} // namespace cm

#endif // __DIRECTX11RENDERER_H__