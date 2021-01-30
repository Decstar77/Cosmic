#pragma once
#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"
#include "src/math/CosmicGeometry.h"

namespace cm
{
	struct MeshMetaData
	{
		String name;
		real32 base_scale;
		int32 face_count;
		int32 position_count;
		int32 normal_count;
		int32 tcoords_count;
		AABB bounding_box;
	};

	class MeshInstance
	{
	public:
		MeshMetaData meta_data;
		int32 asset_table_index = -1;
		int32 graphics_table_index = -1;

		int32 index = -1;

		inline bool IsOnGPU() const { return graphics_table_index >= 0; }

	public:
		MeshInstance() {}
		~MeshInstance() {}

	};


	enum class TextureFileType
	{
		INVALID,
		PNG,
		JPG,
		BMP
	};

	enum class TextureFormat
	{
		R8G8B8A8_UNORM,

	};

	struct TextureMetaData
	{
		TextureFormat format;
		TextureFileType file_type;
		int32 width;
		int32 height;
		int32 channel_count;
	};

	class TextureInstance
	{
	public:
		TextureMetaData meta_data;

		int32 asset_table_index = -1;
		int32 graphics_table_index = -1;
	};


	struct ShaderMetaData
	{
		String path;
		FileResult vertex_file;
		FileResult pixel_file;
	};

	class ShaderInstance
	{
	public:
		ShaderMetaData meta_data;

		int32 asset_table_index = -1;
		int32 graphics_table_index = -1;
	};

	struct Material
	{
		ShaderInstance shader;
		std::vector<TextureInstance> textures;
	};


}