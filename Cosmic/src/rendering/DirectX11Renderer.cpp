#include "DirectX11Renderer.h"
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

	Pipeline::Pipeline()
	{
	}

	Pipeline::~Pipeline()
	{
	}

	DXMesh::DXMesh()
	{
	}

	DXMesh::~DXMesh()
	{
	}



	bool GraphicsContext::InitializeDirectX(HWND window)
	{
		GraphicsContext::window = window;

		D3D_FEATURE_LEVEL feature_level;

		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.RefreshRate.Denominator = 0; //1000;
		sd.BufferDesc.RefreshRate.Numerator = 0;   // 60000;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.SampleDesc.Count = 1;
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

		ID3D11Resource *back_buffer = nullptr;
		swapchain->GetBuffer(0, __uuidof(ID3D11Resource), (void **)&back_buffer);
		//D3D11_RENDER_TARGET_VIEW_DESC rd = {};
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target);

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
		depth_ds.SampleDesc.Count = 1;
		depth_ds.SampleDesc.Quality = 0;
		depth_ds.Usage = D3D11_USAGE_DEFAULT;
		depth_ds.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		device->CreateTexture2D(&depth_ds, nullptr, &depth_texture);

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_view_dsc = {};
		depth_view_dsc.Format = DXGI_FORMAT_D32_FLOAT;
		depth_view_dsc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_view_dsc.Texture2D.MipSlice = 0;
		DXCHECK(device->CreateDepthStencilView(depth_texture, &depth_view_dsc, &depth_target))

			back_buffer->Release();

		ID3D11RasterizerState *rs_state = nullptr;
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

		DXCHECK(device->CreateRasterizerState(&rs_desc, &rs_state));

		context->RSSetState(rs_state);

		return true;
	}

	void GraphicsContext::Destroy()
	{
		DXRELEASE(swapchain);
		DXRELEASE(device);
		DXRELEASE(context);
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
		Mat4f const_buffer_data;
		const_buffer_data = view_matrix * projection_matrix;
		const_buffer_data = Transpose(const_buffer_data);

		DXINFO(context->UpdateSubresource(const_buffer, 0, nullptr, const_buffer_data.ptr, 0, 0));

		D3D11_MAPPED_SUBRESOURCE resource;

		DXCHECK(context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource));
		memcpy(resource.pData, vertex_data, vertex_size_bytes);
		DXINFO(context->Unmap(vertex_buffer, 0));

		RECT rect;
		GetClientRect(window, &rect);

		real32 window_width = (real32)(rect.right - rect.left);
		real32 window_height = (real32)(rect.bottom - rect.top);

		D3D11_VIEWPORT viewport;
		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		DXINFO(context->VSSetShader(pipeline.vs_shader, nullptr, 0));
		DXINFO(context->PSSetShader(pipeline.ps_shader, nullptr, 0));

		DXINFO(context->VSSetConstantBuffers(0, 1, &const_buffer));

		uint32 offset = 0;
		uint32 vertex_stride_bytes = vertex_stride * sizeof(real32);
		DXINFO(context->IASetVertexBuffers(0, 1, &vertex_buffer, &vertex_stride_bytes, &offset));

		DXINFO(context->IASetInputLayout(pipeline.layout));

		DXINFO(context->RSSetViewports(1, &viewport));

		DXINFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST));

		DXINFO(context->OMSetRenderTargets(1, &render_target, depth_target));

		DXINFO(context->Draw(vertex_count, 0));

		ZeroMemory(vertex_data, vertex_size_bytes);
		next_vertex_index = 0;
	}

	DirectXDebugRenderer::DirectXDebugRenderer()
	{
		CreateVertexBuffer();
		CreateConstBuffer();
	}

	DirectXDebugRenderer::~DirectXDebugRenderer()
	{
	}

	void DirectXDebugRenderer::CreateVertexBuffer()
	{
		D3D11_BUFFER_DESC vertex_desc = {};
		vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertex_desc.Usage = D3D11_USAGE_DYNAMIC;
		vertex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertex_desc.MiscFlags = 0;
		vertex_desc.ByteWidth = vertex_size_bytes;
		vertex_desc.StructureByteStride = sizeof(real32) * vertex_stride;

		D3D11_SUBRESOURCE_DATA vertex_res = {};
		vertex_res.pSysMem = vertex_data;

		DXCHECK(device->CreateBuffer(&vertex_desc, &vertex_res, &vertex_buffer));
	}

	void DirectXDebugRenderer::CreateConstBuffer()
	{
		D3D11_BUFFER_DESC cdc = {};
		cdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cdc.Usage = D3D11_USAGE_DEFAULT; //D3D11_USAGE_DYNAMIC;
		cdc.CPUAccessFlags = 0;
		cdc.MiscFlags = 0;
		cdc.ByteWidth = sizeof(Mat4f);
		cdc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA cdc_sub = {};
		cdc_sub.pSysMem = Mat4f(1).ptr;

		DXCHECK(device->CreateBuffer(&cdc, &cdc_sub, &const_buffer));
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

	inline DirectXImmediateRenderer *DirectXImmediateRenderer::GetInstance()
	{
		ASSERT(instance, "Tried to get DirectXImmediateRenderer instance but non exists");

		return instance;
	}

	DirectXImmediateRenderer::DirectXImmediateRenderer(HWND window)
	{
		if (!instance)
		{
			CreateDevice();
			CreateArrays();
			instance = this;
		}
		else
		{
			ASSERT(0, "Trying to create another DirectXImmediateRenderer when one exists.");
		}
	}

	DirectXImmediateRenderer::~DirectXImmediateRenderer()
	{
		DXRELEASE(render_target)
			DXRELEASE(context)
			DXRELEASE(swapchain)
			DXRELEASE(device)
	}

	void DirectXImmediateRenderer::SetMatrices(const Mat4f &view, const Mat4f &proj)
	{
		view_matrix = view;
		projection_matrix = proj;
	}

	void DirectXImmediateRenderer::SetTexture(Texture texture)
	{
		DXINFO(context->PSSetShaderResources(0, 1, &texture.view));
		DXINFO(context->PSSetSamplers(0, 1, &texture.sampler));
	}

	//MeshInstance DirectXImmediateRenderer::CreateMesh(const String &file_name, const real32 &scale)
	//{
	//	// EditableMesh mesh = Win32::LoadOBJModel(file_name, scale);
	//	FileResult file = PlatformLoadFile(file_name);
	//	EditableMesh mesh = OBJParseModel(file, file_name, scale);
	//	// AABB bounding_box = mesh.CalculateBoundingBox();
	//	ModelResult sphere_mesh = mesh.ConvertToPNTFormat();
	//	MeshInstance inst = CreateMesh(sphere_mesh.vertices, sphere_mesh.vertex_size,
	//		sphere_mesh.stride, sphere_mesh.indices, sphere_mesh.index_count);

	//	FreeModel(&sphere_mesh);
	//	PlatformFreeFile(&file);
	//	mesh.Free();

	//	return inst;
	//}

	MeshInstance DirectXImmediateRenderer::CreateMesh(EditableMesh &mesh)
	{
		ModelResult sphere_mesh = mesh.ConvertToPNTFormat();

		MeshInstance inst = CreateMesh(sphere_mesh.vertices, sphere_mesh.vertex_size,
			sphere_mesh.stride, sphere_mesh.indices, sphere_mesh.index_count);

		FreeModel(&sphere_mesh);

		return inst;
	}

	MeshInstance DirectXImmediateRenderer::CreateMesh(real32 *vertex_data, uint32 vertex_size,
		uint32 vertex_stride_bytes, uint32 *index_data, uint32 index_count)
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

		uint32 index = mesh_free_list.top(); mesh_free_list.pop();
		mesh.index = index;

		meshes[index] = mesh;

		MeshInstance mesh_instance(index);

		return mesh_instance;
	}

	Pipeline DirectXImmediateRenderer::CreatePipline(const String &vertex_dir, const String &pixel_dir)
	{
		Pipeline shader;

		shader.vertex_file = Platform::LoadFile(vertex_dir);
		shader.pixel_file = Platform::LoadFile(pixel_dir);

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

		DXCHECK(device->CreateInputLayout(layouts, 3, shader.vertex_file.data.data(), shader.vertex_file.data.size(), &shader.layout));

		DXCHECK(device->CreateVertexShader(shader.vertex_file.data.data(), shader.vertex_file.data.size(), nullptr, &shader.vs_shader));
		DXCHECK(device->CreatePixelShader(shader.pixel_file.data.data(), shader.pixel_file.data.size(), nullptr, &shader.ps_shader));

		return shader;
	}

	//Texture DirectXImmediateRenderer::CreateTexture(const String &file_name)
	//{
	//	ImageResult image = Win32::LoadImage(file_name);
	//	Texture texture = CreateTexture(image.width, image.height, image.channel_count, image.data);
	//	Win32::FreeImage(&image);

	//	return texture;
	//}

	Texture DirectXImmediateRenderer::CreateTexture(uint32 width, uint32 height, uint32 num_channels, uint8 *data)
	{
		struct Colour
		{
			uint8 r;
			uint8 g;
			uint8 b;
			uint8 a;
		};

		// Colour c;
		// c.r = 255;
		// c.g = 0;
		// c.b = 0;
		// c.a = 0;

		// width = 1;
		// height = 1;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = width;
		desc.Height = height;
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
		sd.pSysMem = data;
		sd.SysMemPitch = width * sizeof(uint8) * num_channels;

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

		Texture result;
		result.texture = texture;
		result.sampler = sampler;
		result.view = view;

		return result;
	}

	void DirectXImmediateRenderer::FreeTexture(Texture *texture)
	{
		if (texture->view)
		{
			DXRELEASE(texture->view);
		}
		if (texture->texture)
		{
			DXRELEASE(texture->texture);
		}
		if (texture->sampler)
		{
			DXRELEASE(texture->sampler);
		}
	}

	void DirectXImmediateRenderer::FreeMesh(MeshInstance instance)
	{
		uint32 index = instance.GetIndex();
		DXMesh *mesh = &meshes[index];

		if (mesh->IsValid())
		{
			if (mesh->index == index)
			{
				DXRELEASE(mesh->vertex_buffer);
				DXRELEASE(mesh->index_buffer);
				mesh->index = UINT32_MAX;
				mesh->stride_bytes = UINT32_MAX;
				mesh->index_count = UINT16_MAX;

				mesh_free_list.push(index);
			}
			else
			{
				// @NOTE: This probably shoudn't crash anything.
				ASSERT(0, "Nope it's gone, freeing a mesh that has already been freed");
			}
		}
		else
		{
			// @NOTE: This probably shoudn't crash anything.
			ASSERT(0, "Nope it's gone, freeing a mesh that has already been freed");
		}
	}

	void DirectXImmediateRenderer::RenderMesh(const MeshInstance &mesh_instance, const Transform &transform)
	{
		if (mesh_instance.IsValid())
		{
			Mat4f m = transform.CalculateTransformMatrix();

			Mat4f const_buffer_data[2];
			const_buffer_data[0] = m * view_matrix * projection_matrix;
			const_buffer_data[1] = Inverse(m);

			const_buffer_data[0] = Transpose(const_buffer_data[0]);
			const_buffer_data[1] = Transpose(const_buffer_data[1]);

			//D3D11_MAPPED_SUBRESOURCE map = {};
			//DXCHECK(context->Map(const_buffer, 0, D3D11_MAP_WRITE, 0, &map));

			DXINFO(context->UpdateSubresource(const_buffer, 0, nullptr, const_buffer_data, 0, 0));

			RECT rect;
			GetClientRect(window, &rect);

			real32 window_width = (real32)(rect.right - rect.left);
			real32 window_height = (real32)(rect.bottom - rect.top);

			D3D11_VIEWPORT viewport;
			viewport.Width = window_width;
			viewport.Height = window_height;
			viewport.MinDepth = 0;
			viewport.MaxDepth = 1;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;

			uint32 mesh_index = mesh_instance.GetIndex();
			DXMesh *mesh = &meshes[mesh_index];
			if (mesh->IsValid())
			{
				DXINFO(context->VSSetShader(shader.vs_shader, nullptr, 0));
				DXINFO(context->PSSetShader(shader.ps_shader, nullptr, 0));

				DXINFO(context->VSSetConstantBuffers(0, 1, &const_buffer));

				uint32 offset = 0;
				DXINFO(context->IASetVertexBuffers(0, 1, &mesh->vertex_buffer, &mesh->stride_bytes, &offset));

				DXINFO(context->IASetIndexBuffer(mesh->index_buffer, DXGI_FORMAT_R32_UINT, 0));

				DXINFO(context->IASetInputLayout(shader.layout));

				DXINFO(context->RSSetViewports(1, &viewport));

				DXINFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

				DXINFO(context->OMSetRenderTargets(1, &render_target, depth_target));

				DXINFO(context->DrawIndexed(mesh->index_count, 0, 0));
			}
		}
	}

	void DirectXImmediateRenderer::RenderMesh(const MeshInstance &mesh_instance, const Mat4f &transform_matrix)
	{
		Mat4f const_buffer_data[2];
		const_buffer_data[0] = transform_matrix * view_matrix * projection_matrix;
		const_buffer_data[1] = Inverse(const_buffer_data[0]);

		const_buffer_data[0] = Transpose(const_buffer_data[0]);
		const_buffer_data[1] = Transpose(const_buffer_data[1]);

		//D3D11_MAPPED_SUBRESOURCE map = {};
		//DXCHECK(context->Map(const_buffer, 0, D3D11_MAP_WRITE, 0, &map));

		DXINFO(context->UpdateSubresource(const_buffer, 0, nullptr, const_buffer_data, 0, 0));

		RECT rect;
		GetClientRect(window, &rect);

		real32 window_width = (real32)(rect.right - rect.left);
		real32 window_height = (real32)(rect.bottom - rect.top);

		D3D11_VIEWPORT viewport;
		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		uint32 mesh_index = mesh_instance.GetIndex();
		DXMesh *mesh = &meshes[mesh_index];
		if (mesh->IsValid())
		{
			DXINFO(context->VSSetShader(shader.vs_shader, nullptr, 0));
			DXINFO(context->PSSetShader(shader.ps_shader, nullptr, 0));

			DXINFO(context->VSSetConstantBuffers(0, 1, &const_buffer));

			uint32 offset = 0;
			DXINFO(context->IASetVertexBuffers(0, 1, &mesh->vertex_buffer, &mesh->stride_bytes, &offset));

			DXINFO(context->IASetIndexBuffer(mesh->index_buffer, DXGI_FORMAT_R32_UINT, 0));

			DXINFO(context->IASetInputLayout(shader.layout));

			DXINFO(context->RSSetViewports(1, &viewport));

			DXINFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

			DXINFO(context->OMSetRenderTargets(1, &render_target, depth_target));

			DXINFO(context->DrawIndexed(mesh->index_count, 0, 0));
		}
	}

	void DirectXImmediateRenderer::RenderQuad(const MeshInstance &mesh_instance, const Mat4f &transform)
	{

		DXINFO(context->UpdateSubresource(const_buffer, 0, nullptr, transform.ptr, 0, 0));

		RECT rect;
		GetClientRect(window, &rect);

		real32 window_width = (real32)(rect.right - rect.left);
		real32 window_height = (real32)(rect.bottom - rect.top);

		D3D11_VIEWPORT viewport;
		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		uint32 mesh_index = mesh_instance.GetIndex();
		DXMesh *mesh = &meshes[mesh_index];
		if (mesh->IsValid())
		{
			DXINFO(context->VSSetShader(shader.vs_shader, nullptr, 0));
			DXINFO(context->PSSetShader(shader.ps_shader, nullptr, 0));

			DXINFO(context->VSSetConstantBuffers(0, 1, &const_buffer));

			uint32 offset = 0;
			DXINFO(context->IASetVertexBuffers(0, 1, &mesh->vertex_buffer, &mesh->stride_bytes, &offset));

			DXINFO(context->IASetIndexBuffer(mesh->index_buffer, DXGI_FORMAT_R32_UINT, 0));

			DXINFO(context->IASetInputLayout(shader.layout));

			DXINFO(context->RSSetViewports(1, &viewport));

			DXINFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

			DXINFO(context->OMSetRenderTargets(1, &render_target, depth_target));

			DXINFO(context->DrawIndexed(mesh->index_count, 0, 0));
		}
		else
		{
			LOG("Mesh invalid");
		}
	}

	void DirectXImmediateRenderer::LOGGPUCards()
	{
		IDXGIFactory *factory = nullptr;
		DXCHECK(CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory));

		IDXGIAdapter *adapter = nullptr;

		uint32 index = 0;
		factory->EnumAdapters(index, &adapter);

		// while(SUCCEEDED())
		// {
		//     DXGI_ADAPTER_DESC desc = {};
		//     adapter->GetDesc(&desc);
		//     index++;
		// }

		index = 0;
		IDXGIOutput *output = nullptr;
		adapter->EnumOutputs(index, &output);
		// while (SUCCEEDED(adapter->EnumOutputs(index, &output)))
		// {
		//     DXGI_OUTPUT_DESC desc = {};
		//     output->GetDesc(&desc);
		//     index++;
		// }

		uint32 modes_count = 0;
		DXCHECK(output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modes_count, NULL));

		DXGI_MODE_DESC *modes = new DXGI_MODE_DESC[modes_count];
		output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modes_count, modes);

		//DXGI_OUTPUT_DESC desc = {};
		//output->GetDesc(&desc);
		// LOG(desc.DeviceName);
		for (int32 i = 0; i < (int32)modes_count; i++)
		{
			DXGI_MODE_DESC mode = modes[i];

			if (mode.Width == (unsigned int)1280)
			{
				int b = 1;
				if (mode.Height == (unsigned int)720)
				{
					int a = 2;
					//numerator = displayModeList[i].RefreshRate.Numerator;
					//denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}
	}

	void DirectXImmediateRenderer::ClearBuffer(const Vec4f &colour)
	{
		context->ClearRenderTargetView(render_target, colour.ptr);
		context->ClearDepthStencilView(depth_target, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void DirectXImmediateRenderer::EndFrame()
	{
		swapchain->Present(1, 0);
	}

	void DirectXImmediateRenderer::CreateDevice()
	{
		D3D11_BUFFER_DESC cdc = {};
		cdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cdc.Usage = D3D11_USAGE_DEFAULT; //D3D11_USAGE_DYNAMIC;
		cdc.CPUAccessFlags = 0;
		cdc.MiscFlags = 0;
		cdc.ByteWidth = (uint32)(sizeof(Mat4f) * 2.0f);
		cdc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA cdc_sub = {};
		Mat4f dummy[] = { Mat4f(1), Mat4f(1) };
		cdc_sub.pSysMem = dummy;

		DXCHECK(device->CreateBuffer(&cdc, &cdc_sub, &const_buffer));
	}

	void DirectXImmediateRenderer::CreateArrays()
	{
		meshes.resize(total_mesh_count);
		for (uint32 i = 0; i < total_mesh_count; i++)
		{
			mesh_free_list.push(total_mesh_count - i - 1);
		}
	}
} // namespace cm