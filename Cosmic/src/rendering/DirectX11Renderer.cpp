#include "DirectX11Renderer.h"
namespace cm
{
	bool GraphicsContext::InitializeDirectX(HWND window)
	{
		GraphicsContext::window = window;

		D3D_FEATURE_LEVEL feature_level;

		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.RefreshRate.Denominator = 0; //1000;
		sd.BufferDesc.RefreshRate.Numerator = 0;   // 60000;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		sd.SampleDesc.Count = msaa_sample_count;//4;
		sd.SampleDesc.Quality = 0;

		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1; // @NOTE This implies just the back buffer, ie we have two buffers
		sd.OutputWindow = window;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		DXCHECK(D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			D3D11_CREATE_DEVICE_DEBUG,
			NULL, 0, // @NOTE: Pick the highest feature level
			D3D11_SDK_VERSION,
			&sd,
			&swapchain,
			&device,
			&feature_level,
			&context));


		//uint32 levels;
		//device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &levels);


		ID3D11Resource *back_buffer = nullptr;
		swapchain->GetBuffer(0, __uuidof(ID3D11Resource), (void **)&back_buffer);
		//D3D11_RENDER_TARGET_VIEW_DESC rd = {};
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target);
		back_buffer->Release();

		D3D11_DEPTH_STENCIL_DESC ds = {};
		ds.DepthEnable = TRUE;
		ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // @NOTE: Check docs, turns off depth buffer D3D11_DEPTH_WRITE_MASK_ZERO
		ds.StencilEnable = FALSE;
		ds.DepthFunc = D3D11_COMPARISON_LESS;

		ID3D11DepthStencilState *ds_state = nullptr;
		DXCHECK(device->CreateDepthStencilState(&ds, &ds_state));

		context->OMSetDepthStencilState(ds_state, 1);

		RECT window_rect;
		GetClientRect(window, &window_rect);

		ID3D11Texture2D *depth_texture = nullptr;
		D3D11_TEXTURE2D_DESC depth_ds = {};
		depth_ds.Width = (uint32)(window_rect.right - window_rect.left);
		depth_ds.Height = (uint32)(window_rect.bottom - window_rect.top);
		depth_ds.MipLevels = 1;
		depth_ds.ArraySize = 1;
		depth_ds.Format = DXGI_FORMAT_D32_FLOAT;
		depth_ds.SampleDesc.Count = msaa_sample_count;
		depth_ds.SampleDesc.Quality = 0;
		depth_ds.Usage = D3D11_USAGE_DEFAULT;
		depth_ds.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		device->CreateTexture2D(&depth_ds, nullptr, &depth_texture);

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_view_dsc = {};
		depth_view_dsc.Format = DXGI_FORMAT_D32_FLOAT;
		depth_view_dsc.ViewDimension = IsMSAAEnabled() ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_view_dsc.Texture2D.MipSlice = 0;
		DXCHECK(device->CreateDepthStencilView(depth_texture, &depth_view_dsc, &depth_target));

		swapchain_render_target.depth_target = depth_target;
		swapchain_render_target.depth_texture = depth_texture;
		swapchain_render_target.render_target = render_target;
		swapchain_render_target.texture = nullptr;
		swapchain_render_target.shader_view = nullptr;


		D3D11_RASTERIZER_DESC rs_desc = {};
		rs_desc.FillMode = D3D11_FILL_SOLID;
		rs_desc.CullMode = D3D11_CULL_BACK;
		rs_desc.FrontCounterClockwise = FALSE;
		rs_desc.DepthBias = 0;
		rs_desc.DepthBiasClamp = 1.0f;
		rs_desc.SlopeScaledDepthBias = 0.0f;
		rs_desc.DepthClipEnable = TRUE;
		rs_desc.MultisampleEnable = FALSE;
		rs_desc.AntialiasedLineEnable = FALSE;

		DXCHECK(device->CreateRasterizerState(&rs_desc, &rs_standard_state));

		context->RSSetState(rs_standard_state);

		rs_desc = {};
		rs_desc.FillMode = D3D11_FILL_SOLID;
		rs_desc.CullMode = D3D11_CULL_FRONT;
		rs_desc.FrontCounterClockwise = FALSE;
		rs_desc.DepthBias = 0;
		rs_desc.DepthBiasClamp = 1.0f;
		rs_desc.SlopeScaledDepthBias = 0.0f;
		rs_desc.DepthClipEnable = TRUE;
		rs_desc.MultisampleEnable = FALSE;
		rs_desc.AntialiasedLineEnable = FALSE;

		DXCHECK(device->CreateRasterizerState(&rs_desc, &rs_shadow_state));
		return true;
	}

	void GraphicsContext::Destroy()
	{
		DXRELEASE(swapchain);
		DXRELEASE(device);
		DXRELEASE(context);
	}

	void GraphicsContext::CreateMesh(MeshInstance *instance, EditableMesh *editable_mesh)
	{
		ModelResult mesh = editable_mesh->ConvertToPNTFormat();

		int32 areana_index = CreateMesh(mesh.vertices.data(), mesh.vertex_size,
			mesh.stride, mesh.indices.data(), mesh.index_count);

		instance->graphics_table_index = areana_index;
	}

	int32 GraphicsContext::CreateMesh(real32 *vertex_data, uint32 vertex_size, uint32 vertex_stride_bytes, uint32 *index_data, uint32 index_count)
	{
		D3D11_BUFFER_DESC vertex_desc = {};
		vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertex_desc.Usage = D3D11_USAGE_DEFAULT;
		vertex_desc.CPUAccessFlags = 0;
		vertex_desc.MiscFlags = 0;
		vertex_desc.ByteWidth = vertex_size * sizeof(real32);
		vertex_desc.StructureByteStride = vertex_stride_bytes;

		D3D11_SUBRESOURCE_DATA vertex_res = {};
		vertex_res.pSysMem = vertex_data;

		D3D11_BUFFER_DESC index_desc = {};
		index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		index_desc.Usage = D3D11_USAGE_DEFAULT;
		index_desc.CPUAccessFlags = 0;
		index_desc.MiscFlags = 0;
		index_desc.ByteWidth = index_count * sizeof(uint32);
		index_desc.StructureByteStride = sizeof(uint32);

		D3D11_SUBRESOURCE_DATA index_res = {};
		index_res.pSysMem = index_data;

		DXMesh mesh;
		mesh.index_count = index_count;
		mesh.stride_bytes = vertex_stride_bytes;
		mesh.vertex_size = vertex_size;
		DXCHECK(device->CreateBuffer(&vertex_desc, &vertex_res, &mesh.vertex_buffer));
		DXCHECK(device->CreateBuffer(&index_desc, &index_res, &mesh.index_buffer));

		int32 index = static_cast<int32>(mesh_areana.Add(mesh));

		return index;
	}

	void GraphicsContext::CreateShader(ShaderInstance *instance, EditableShader *editable_shader)
	{
		DXShader shader;

		D3D11_INPUT_ELEMENT_DESC pos_desc = {};
		pos_desc.SemanticName = "Position";
		pos_desc.SemanticIndex = 0;
		pos_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		pos_desc.InputSlot = 0;
		pos_desc.AlignedByteOffset = 0;
		pos_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pos_desc.InstanceDataStepRate = 0;

		D3D11_INPUT_ELEMENT_DESC nrm_desc = {};
		nrm_desc.SemanticName = "Normal";
		nrm_desc.SemanticIndex = 0;
		nrm_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		nrm_desc.InputSlot = 0;
		nrm_desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		nrm_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		nrm_desc.InstanceDataStepRate = 0;

		D3D11_INPUT_ELEMENT_DESC txc_desc = {};
		txc_desc.SemanticName = "TexCord";
		txc_desc.SemanticIndex = 0;
		txc_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
		txc_desc.InputSlot = 0;
		txc_desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		txc_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		txc_desc.InstanceDataStepRate = 0;

		D3D11_INPUT_ELEMENT_DESC layouts[] = { pos_desc, nrm_desc, txc_desc };

		DXCHECK(device->CreateInputLayout(layouts, 3, editable_shader->vertex_file.data.data(),
			editable_shader->vertex_file.data.size(), &shader.layout));

		DXCHECK(device->CreateVertexShader(editable_shader->vertex_file.data.data(),
			editable_shader->vertex_file.data.size(), nullptr, &shader.vs_shader));

		DXCHECK(device->CreatePixelShader(editable_shader->pixel_file.data.data(),
			editable_shader->pixel_file.data.size(), nullptr, &shader.ps_shader));

		int32 index = static_cast<int32>(shader_areana.Add(shader));

		instance->graphics_table_index = index;
	}

	void GraphicsContext::CreateTexture(TextureInstance *instance, EditableTexture *edtiable_texture)
	{
		struct Colour
		{
			uint8 r;
			uint8 g;
			uint8 b;
			uint8 a;
		};

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = instance->meta_data.width;
		desc.Height = instance->meta_data.height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = edtiable_texture->pixel_data.data();
		sd.SysMemPitch = instance->meta_data.width * sizeof(uint8) * instance->meta_data.channel_count;

		ID3D11Texture2D *texture = nullptr;
		DXCHECK(device->CreateTexture2D(&desc, &sd, &texture));

		D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
		view_desc.Format = desc.Format;
		view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		view_desc.Texture2D.MostDetailedMip = 0;
		view_desc.Texture2D.MipLevels = 1;

		ID3D11ShaderResourceView *view = nullptr;
		DXCHECK(device->CreateShaderResourceView(texture, &view_desc, &view));

		ID3D11SamplerState *sampler = nullptr;
		D3D11_SAMPLER_DESC sam_desc = {};
		sam_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sam_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sam_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sam_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		DXCHECK(device->CreateSamplerState(&sam_desc, &sampler));

		DXTexture text;
		text.texture = texture;
		text.sampler = sampler;
		text.view = view;

		int32 index = static_cast<int32>(texture_areana.Add(text));

		instance->graphics_table_index = index;
	}



	void GraphicsContext::Present()
	{
		PROFILE_FUNCTION();
		DXCHECK(swapchain->Present(1, 0));
	}

	void GraphicsContext::ClearBuffer(const Vec4f &colour)
	{
		DXINFO(context->ClearRenderTargetView(render_target, colour.ptr));
		DXINFO(context->ClearDepthStencilView(depth_target, D3D11_CLEAR_DEPTH, 1.0f, 0));
	}

	void GraphicsContext::LogGPUS()
	{
		IDXGIFactory* gi_factory;
		DXCHECK(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&gi_factory)));

		std::vector<IDXGIAdapter*> adapters;

		{
			IDXGIAdapter* temp;
			for (uint32 adapter_index = 0;
				gi_factory->EnumAdapters(adapter_index, &temp) != DXGI_ERROR_NOT_FOUND;
				adapter_index++)
			{
				adapters.push_back(temp);
			}
		}

		for (IDXGIAdapter* adapter : adapters)
		{
			DXGI_ADAPTER_DESC desc = {};
			DXCHECK(adapter->GetDesc(&desc));

			LARGE_INTEGER version;
			bool dx_support = adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &version) == S_OK;

			std::vector<IDXGIOutput*> displays;
			{
				IDXGIOutput* temp;
				for (uint32 display_index = 0;
					adapter->EnumOutputs(display_index, &temp) != DXGI_ERROR_NOT_FOUND;
					display_index++)
				{
					displays.push_back(temp);
				}
			}



			std::cout << "----------------" << std::endl;
			std::wcout << desc.Description << std::endl;
			std::cout << "Supports DX: " << dx_support << std::endl;
			std::cout << "VRAM: " << desc.DedicatedVideoMemory / (1000 * 1000) << " MBS" << std::endl;
			std::cout << "SRAM: " << desc.DedicatedSystemMemory / (1000 * 1000) << " MBS" << std::endl;

			for (IDXGIOutput *display : displays)
			{
				DXGI_OUTPUT_DESC display_desc;

				DXCHECK(display->GetDesc(&display_desc));

				std::wcout << display_desc.DeviceName << std::endl;

				DXRELEASE(display);
			}

			std::cout << "----------------" << std::endl;

			DXRELEASE(adapter);
		}


		DXRELEASE(gi_factory);
	}

	GraphicsContext::GraphicsContext()
	{

	}

	GraphicsContext::~GraphicsContext()
	{

	}

	void DirectXDebugRenderer::SetMatrices(const Mat4f &view, const Mat4f &proj)
	{
		view_matrix = view;
		projection_matrix = proj;
	}

	void DirectXDebugRenderer::PushNewLine(const Vec3f &p0, const Vec3f &p1)
	{
		PushLine(p0, p1);
	}

	void DirectXDebugRenderer::PushNewLine(const Vec4f &p0, const Vec4f &p1)
	{
		PushLine(p0, p1);
	}

	void DirectXDebugRenderer::RenderAndFlush()
	{
		GETDEUBBGER();

		D3D11_MAPPED_SUBRESOURCE resource;

		DXCHECK(gc->context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource));
		memcpy(resource.pData, vertex_data, vertex_size_bytes);
		DXINFO(gc->context->Unmap(vertex_buffer, 0));

		uint32 offset = 0;
		uint32 vertex_stride_bytes = vertex_stride * sizeof(real32);
		DXINFO(gc->context->IASetVertexBuffers(0, 1, &vertex_buffer, &vertex_stride_bytes, &offset));

		//////////////////////////////////
		//////////////////////////////////

		DXShader *shader = gc->shader_areana.Get(shader_instance.graphics_table_index);
		shader->Bind(gc);

		//////////////////////////////////
		//////////////////////////////////

		DXINFO(gc->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST));

		DXINFO(gc->context->Draw(vertex_count, 0));

		DXINFO(gc->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		//////////////////////////////////
		//////////////////////////////////

		ZeroMemory(vertex_data, vertex_size_bytes);
		next_vertex_index = 0;
	}

	DirectXDebugRenderer::DirectXDebugRenderer(GraphicsContext *graphics_context)
		: gc(graphics_context)
	{
		CreateVertexBuffer();
		CreateConstBuffer();
	}

	DirectXDebugRenderer::~DirectXDebugRenderer()
	{
	}

	void DirectXDebugRenderer::CreateVertexBuffer()
	{
		GETDEUBBGER();

		D3D11_BUFFER_DESC vertex_desc = {};
		vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertex_desc.Usage = D3D11_USAGE_DYNAMIC;
		vertex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertex_desc.MiscFlags = 0;
		vertex_desc.ByteWidth = vertex_size_bytes;
		vertex_desc.StructureByteStride = sizeof(real32) * vertex_stride;

		D3D11_SUBRESOURCE_DATA vertex_res = {};
		vertex_res.pSysMem = vertex_data;

		DXCHECK(gc->device->CreateBuffer(&vertex_desc, &vertex_res, &vertex_buffer));
	}

	void DirectXDebugRenderer::CreateConstBuffer()
	{
		GETDEUBBGER();

		D3D11_BUFFER_DESC cdc = {};
		cdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cdc.Usage = D3D11_USAGE_DEFAULT; //D3D11_USAGE_DYNAMIC;
		cdc.CPUAccessFlags = 0;
		cdc.MiscFlags = 0;
		cdc.ByteWidth = sizeof(Mat4f);
		cdc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA cdc_sub = {};
		cdc_sub.pSysMem = Mat4f(1).ptr;

		DXCHECK(gc->device->CreateBuffer(&cdc, &cdc_sub, &const_buffer));
	}

	void DirectXDebugRenderer::PushLine(const Vec3f &a, const Vec3f &b)
	{
		ASSERT((next_vertex_index - 6) < vertex_count * vertex_stride, "We've run out of space for out debug renderer buffer");
		vertex_data[next_vertex_index] = a.x;
		vertex_data[next_vertex_index + 1] = a.y;
		vertex_data[next_vertex_index + 2] = a.z;
		next_vertex_index += vertex_stride;
		vertex_data[next_vertex_index] = b.x;
		vertex_data[next_vertex_index + 1] = b.y;
		vertex_data[next_vertex_index + 2] = b.z;
		next_vertex_index += vertex_stride;
	}

	void DirectXDebugRenderer::PushLine(const Vec4f &a, const Vec4f &b)
	{
		ASSERT((next_vertex_index - 6) < vertex_count * vertex_stride, "We've run out of space for out debug renderer buffer");
		vertex_data[next_vertex_index] = a.x;
		vertex_data[next_vertex_index + 1] = a.y;
		vertex_data[next_vertex_index + 2] = a.z;
		next_vertex_index += vertex_stride;
		vertex_data[next_vertex_index] = b.x;
		vertex_data[next_vertex_index + 1] = b.y;
		vertex_data[next_vertex_index + 2] = b.z;
		next_vertex_index += vertex_stride;
	}

	void WorldRenderer::RenderWorld(World *world)
	{
		PROFILE_FUNCTION();

		GETDEUBBGER();

		AssetTable *asset_table = GameState::GetAssetTable();

		//material.shader = depth_writer_shader;

		std::vector<Entity *> renderable_entities = world->CreateCollection([](Entity *entity) {
			return entity->active && entity->should_draw && entity->GetMesh().IsOnGPU();
		});

		UploadLightingInfo(world);

		DXINFO(gc->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)); // @TODO: once off

		ID3D11ShaderResourceView* nulltexture[] = { nullptr, nullptr };
		DXINFO(gc->context->PSSetShaderResources(0, 2, nulltexture));

		std::vector<Entity *> direction_lights = world->CreateCollection([](Entity *entity) {
			return entity->active && entity->GetType() == EntityType::DIRECTIONALLIGHT;
		});

		if (direction_lights.size() > 0)
		{
			ShadowPass(renderable_entities, (DirectionalLight*)direction_lights.at(0));
		}

		gc->swapchain_render_target.Clear(gc, Vec4f(1.0f));
		gc->swapchain_render_target.Bind(gc);

		Material material;
		material.shader = asset_table->shader_instances.at(0);
		material.albedo_texture = asset_table->texture_instances.at(0);

		DXINFO(gc->context->PSSetShaderResources(1, 1, &shadow_render_target.shader_view));
		DXINFO(gc->context->PSSetSamplers(1, 1, &shadow_sampler));

		D3D11_VIEWPORT current_viewport = viewport;

		DXINFO(gc->context->RSSetViewports(1, &current_viewport));

		//offscreen_render_target.Clear(gc, Vec4f(0.45f, 0.65f, 0.85f, 1.0f));
		//offscreen_render_target.Bind(gc);

		for (Entity *entity : renderable_entities)
		{
			RenderMesh(entity->GetMesh(), material, entity->GetGlobalTransform());
		}

		if (debug_renderer)
		{
			debug_renderer->RenderAndFlush();
		}

		current_viewport.Width = 480;
		current_viewport.Height = 480;
		current_viewport.MinDepth = 0;
		current_viewport.MaxDepth = 1;
		current_viewport.TopLeftX = 0;
		current_viewport.TopLeftY = 0;

		DXINFO(gc->context->RSSetViewports(1, &current_viewport));

		//gc->swapchain_render_target.Bind(gc);
		//DXINFO(gc->context->PSSetShaderResources(0, 1, &offscreen_render_target.shader_view));

		DXShader *shader = gc->shader_areana.Get(post_processing_shader.graphics_table_index);
		shader->Bind(gc);
		DXINFO(gc->context->PSSetShaderResources(0, 1, &shadow_render_target.shader_view));
		RenderQuad();
	}

	void WorldRenderer::RenderQuad()
	{
		GETDEUBBGER();

		//////////////////////////////////
		//////////////////////////////////

		DXMesh *mesh = gc->mesh_areana.Get(screen_space_quad_index);
		mesh->Bind(gc);

		//////////////////////////////////
		//////////////////////////////////

		//////////////////////////////////
		//////////////////////////////////

		DXINFO(gc->context->DrawIndexed(mesh->index_count, 0, 0));
	}

	void WorldRenderer::ShadowPass(const std::vector<Entity*> &entities, DirectionalLight *light)
	{
		GETDEUBBGER();

		Material material;
		material.shader = depth_writer_shader;

		shadow_render_target.Clear(gc, Vec4f(0.0f));
		shadow_render_target.Bind(gc);

		D3D11_VIEWPORT current_viewport;
		current_viewport.Width = (real32)shadow_render_target.width;
		current_viewport.Height = (real32)shadow_render_target.height;
		current_viewport.MinDepth = 0;
		current_viewport.MaxDepth = 1;
		current_viewport.TopLeftX = 0;
		current_viewport.TopLeftY = 0;

		Mat4f old_view = view_matrix;
		Mat4f old_proj = projection_matrix;

		view_matrix = light->CalculateLightView();
		projection_matrix = light->CalculateLightProjection();

		vs_lighting_buffer.ResetCopyPtr();
		vs_lighting_buffer.CopyInMat4f(Transpose(view_matrix * projection_matrix));
		vs_lighting_buffer.CopyInMat4f(Transpose(view_matrix));
		vs_lighting_buffer.Upload(gc);

		DXINFO(gc->context->RSSetViewports(1, &current_viewport));
		//DXINFO(gc->context->RSSetState(gc->rs_shadow_state));

		for (Entity *entity : entities)
		{
			RenderMesh(entity->GetMesh(), material, entity->GetGlobalTransform());
		}

		view_matrix = old_view;
		projection_matrix = old_proj;

		//DXINFO(gc->context->RSSetState(gc->rs_standard_state));
	}

	void WorldRenderer::RenderMesh(const MeshInstance &mesh_instance, const Material &material, const Transform &transform)
	{
		// @TODO: Error checking

		GETDEUBBGER();

		//////////////////////////////////
		//////////////////////////////////

		// @TODO: Abstract
		Mat4f m = transform.CalculateTransformMatrix();

		Mat4f const_buffer_data[5];
		const_buffer_data[0] = m * view_matrix * projection_matrix;
		const_buffer_data[1] = projection_matrix;
		const_buffer_data[2] = view_matrix;
		const_buffer_data[3] = m;
		const_buffer_data[4] = Inverse(m);

		const_buffer_data[0] = Transpose(const_buffer_data[0]);
		const_buffer_data[1] = Transpose(const_buffer_data[1]);
		const_buffer_data[2] = Transpose(const_buffer_data[2]);
		const_buffer_data[3] = Transpose(const_buffer_data[3]);
		const_buffer_data[4] = Transpose(const_buffer_data[4]);

		DXINFO(gc->context->UpdateSubresource(uniform_buffer, 0, nullptr, const_buffer_data, 0, 0));

		//////////////////////////////////
		//////////////////////////////////

		DXShader *shader = gc->shader_areana.Get(material.shader.graphics_table_index);
		shader->Bind(gc);

		DXINFO(gc->context->VSSetConstantBuffers(0, 1, &uniform_buffer));

		//////////////////////////////////
		//////////////////////////////////

		ASSERT(mesh_instance.IsOnGPU(), "Mesh is not on GPU");
		DXMesh *mesh = gc->mesh_areana.Get(mesh_instance.graphics_table_index);
		mesh->Bind(gc);

		//////////////////////////////////
		//////////////////////////////////

		if (material.albedo_texture.IsValid())
		{
			uint32 index = static_cast<uint32>(material.albedo_texture.graphics_table_index);
			DXTexture *texture = gc->texture_areana.Get(index);
			texture->Bind(gc, 0);
		}

		//////////////////////////////////
		//////////////////////////////////

		DXINFO(gc->context->DrawIndexed(mesh->index_count, 0, 0));
	}

	void WorldRenderer::SetMatrices(const Mat4f &view, const Mat4f &proj)
	{
		view_matrix = view;
		projection_matrix = proj;
	}

	WorldRenderer::WorldRenderer(GraphicsContext *graphics_context)
		: gc(graphics_context)
	{
		// @TODO: Clean up

		GETDEUBBGER();

		D3D11_BUFFER_DESC cdc = {};
		cdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cdc.Usage = D3D11_USAGE_DEFAULT; //D3D11_USAGE_DYNAMIC;
		cdc.CPUAccessFlags = 0;
		cdc.MiscFlags = 0;
		cdc.ByteWidth = (uint32)(sizeof(Mat4f) * 5.0f);
		cdc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA cdc_sub = {};
		Mat4f dummy[] = { Mat4f(1), Mat4f(1), Mat4f(1), Mat4f(1), Mat4f(1) };
		cdc_sub.pSysMem = dummy;

		DXCHECK(gc->device->CreateBuffer(&cdc, &cdc_sub, &uniform_buffer));

		//----------------------------
		//----------------------------
		//----------------------------

		//ps_lighting_buffer.Create(gc, (uint32)(sizeof(Vec3f) * 64));
		ps_lighting_buffer.Create(gc, (uint32)(sizeof(Vec3f) * 65));
		ps_lighting_buffer.ResetCopyPtr();
		ps_lighting_buffer.CopyInVec3f(Vec3f(1.0f));
		ps_lighting_buffer.CopyInVec4f(Vec4f(0.0f));
		ps_lighting_buffer.CopyInVec3f(Vec3f(0.0f));
		ps_lighting_buffer.CopyInVec3f(Vec3f(0.0f));
		ps_lighting_buffer.CopyInVec3f(Vec3f(0.0f));
		ps_lighting_buffer.Upload(gc);
		ps_lighting_buffer.Bind(gc, ShaderStage::PIXEL_SHADER, 0);


		vs_lighting_buffer.Create(gc, sizeof(Mat4f) * 2);
		vs_lighting_buffer.ResetCopyPtr();
		vs_lighting_buffer.CopyInMat4f(Mat4f(1));
		vs_lighting_buffer.CopyInMat4f(Mat4f(1));
		vs_lighting_buffer.Upload(gc);
		vs_lighting_buffer.Bind(gc, ShaderStage::VERTEX_SHADER, 1);

		//----------------------------
		//----------------------------
		//----------------------------

		D3D11_SAMPLER_DESC shadow_sampler_desc = {};
		shadow_sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		shadow_sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		shadow_sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		shadow_sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		DXCHECK(gc->device->CreateSamplerState(&shadow_sampler_desc, &shadow_sampler));
		DXINFO(gc->context->PSSetSamplers(1, 1, &shadow_sampler));

		//----------------------------
		//----------------------------
		//----------------------------



		// @TODO: Use platform
		RECT rect;
		GetClientRect(gc->window, &rect);

		real32 window_width = (real32)(rect.right - rect.left);
		real32 window_height = (real32)(rect.bottom - rect.top);

		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		real32 quad_data[] = {
			-1, 1, 0,	0, 0, -1,	0, 0,
			1, -1, 0,	0, 0, -1,	1, 1,
			-1, -1, 0,	0, 0, -1,	0, 1,
			1, 1, 0,	0, 0, -1,	1, 0
		};

		uint32 index_data[] = {
			0, 1, 2, 0, 3, 1
		};

		screen_space_quad_index = gc->CreateMesh(quad_data, 32, 8 * sizeof(real32), index_data, 6);

		offscreen_render_target.Create(gc, (uint32)window_width, (uint32)window_height);
		//shadow_render_target.Create(gc, (uint32)window_width, (uint32)window_height);
		shadow_render_target.Create(gc, 4096, 4096);
	}

	WorldRenderer::~WorldRenderer()
	{

	}

	void WorldRenderer::UploadLightingInfo(World *world)
	{
		std::vector<Entity *> direction_lights = world->CreateCollection([](Entity *entity) {
			return entity->active && entity->GetType() == EntityType::DIRECTIONALLIGHT;
		});

		std::vector<Entity *> spot_lights = world->CreateCollection([](Entity *entity) {
			return entity->active && entity->GetType() == EntityType::SPOTLIGHT;
		});

		std::vector<Entity *> point_lights = world->CreateCollection([](Entity *entity) {
			return entity->active && entity->GetType() == EntityType::POINTLIGHT;
		});

		const int32 max_directional_light_count = 2;
		const int32 max_spot_light_count = 8;
		const int32 max_point_light_count = 16;

		int32 directional_light_count = static_cast<int32>(direction_lights.size());
		int32 spot_light_count = static_cast<int32>(spot_lights.size());
		int32 point_light_count = static_cast<int32>(point_lights.size());

		Vec3f view_pos = Camera::GetActiveCamera()->GetGlobalPosition();
		Vec3<int32> light_counts = Vec3i(directional_light_count, spot_light_count, point_light_count);

		ps_lighting_buffer.ResetCopyPtr();
		ps_lighting_buffer.CopyInVec3f(view_pos);
		ps_lighting_buffer.CopyInVec4f(Vec4f(GameState::light_size, GameState::blocker_size,
			GameState::blocker_size, GameState::blocker_size));
		ps_lighting_buffer.CopyInVec3i(light_counts);

		{
			int32 directional_light_index = 0;
			for (; directional_light_index < directional_light_count; directional_light_index++)
			{
				DirectionalLight *directional_light =
					reinterpret_cast<DirectionalLight*>(direction_lights.at(directional_light_index));

				Transform t = directional_light->GetGlobalTransform();

				ps_lighting_buffer.CopyInVec3f(t.GetBasis().forward);
				ps_lighting_buffer.CopyInVec3f(directional_light->colour * directional_light->strength);
			}
			for (; directional_light_index < max_directional_light_count; directional_light_index++)
			{
				ps_lighting_buffer.CopyInVec3f(Vec3f(0.0f));
				ps_lighting_buffer.CopyInVec3f(Vec3f(0.0f));
			}
		}
		{
			int32 spot_light_index = 0;
			for (; spot_light_index < spot_light_count; spot_light_index++)
			{
				SpotLight *spot_light =
					reinterpret_cast<SpotLight *>(spot_lights.at(spot_light_index));

				Transform t = spot_light->GetGlobalTransform();

				Vec4f pos_inner = Vec4f(t.position, Cos(DegToRad(spot_light->innner)));
				Vec4f dir_outter = Vec4f(t.GetBasis().forward, Cos(DegToRad(spot_light->outter)));

				ps_lighting_buffer.CopyInVec4f(pos_inner);
				ps_lighting_buffer.CopyInVec4f(dir_outter);
				ps_lighting_buffer.CopyInVec3f(spot_light->colour * spot_light->strength);
			}
			for (; spot_light_index < max_spot_light_count; spot_light_index++)
			{
				ps_lighting_buffer.CopyInVec4f(Vec4f(0.0f));
				ps_lighting_buffer.CopyInVec4f(Vec4f(0.0f));
				ps_lighting_buffer.CopyInVec3f(Vec3f(0.0f));
			}
		}
		{
			int32 point_light_index = 0;
			for (; point_light_index < point_light_count; point_light_index++)
			{
				PointLight *point_light =
					reinterpret_cast<PointLight *>(point_lights.at(point_light_index));
				ps_lighting_buffer.CopyInVec3f(point_light->GetGlobalPosition());
				ps_lighting_buffer.CopyInVec3f(point_light->colour * point_light->strength);
			}
			for (; point_light_index < max_point_light_count; point_light_index++)
			{
				ps_lighting_buffer.CopyInVec3f(Vec3f(0.0f));
				ps_lighting_buffer.CopyInVec3f(Vec3f(0.0f));
			}
		}

		ps_lighting_buffer.Upload(gc);
	}




} // namespace cm