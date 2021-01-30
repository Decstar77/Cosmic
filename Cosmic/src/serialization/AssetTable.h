#pragma once

#include "src/serialization/Assets.h"

#include "src/core/JobSystem.h"
#include "src/serialization/FileParsers.h"

namespace cm
{
#define THREADED 1

	class AssetTable
	{
	public:

		std::vector<MeshInstance> mesh_intances;		// @TODO: Pre reserve this!!
		std::vector<EditableMesh> editable_meshes;		// @TODO: Pre reserve this!!

		std::vector<ShaderInstance> shader_instances;	// @TODO: Pre reserve this!!
		std::vector<EditableShader> editable_shaders;	// @TODO: Pre reserve this!!

		std::vector<TextureInstance> texture_instances;	// @TODO: Pre reserve this!!
		std::vector<EditableTexture> editable_textures;	// @TODO: Pre reserve this!!


		//************************************
		//
		//************************************
		AssetTable() {};
		~AssetTable() {};

	public:
		//************************************
		// @NOTE: Asssumes the mesh is valid and is there
		// @NOTE: This method blocks if locked
		//************************************
		EditableMesh GetEditableMesh(const MeshInstance &instance);


		//************************************
		// @NOTE: Is garuateed to be there with no blocking becaue we load this on main ahead of time
		// @Speed: O(n) as we have to loop through everything. Could make this faster with hash map
		//************************************
		MeshInstance FindMeshInstance(const String &name);
	};


	class ModelLoadJob : public JobWork
	{
	public:
		virtual void Process() override;

	public:
		ModelLoadJob(const FileResult &file_result, const MeshInstance &mesh_instance, AssetTable *asset_table);
		~ModelLoadJob();

	private:
		AssetTable *asset_table;
		FileResult mesh_file;
		MeshInstance mesh_instance;

	};

	class AssetLoader
	{
	public:
		void LoadModel(const String &path);
		void LoadShader(const String &vertex_path, const String &pixel_path);
		void LoadTexture(const String &path);

		void LoadEntireFolderOfModels(const String &path);
		void LoadEnitreFolderOfTextures(const String &path);

	public:
		AssetLoader(AssetTable *asset_table, const String &resource_path);
		~AssetLoader();

	private:
		String resource_path;
		AssetTable *asset_table;

		std::list<ModelLoadJob> model_jobs;

	private:
		void DoLoadEntireFolderOfModels(FolderResult mesh_folder);
		void ParseModel(const FileResult &mesh_file, const MeshInstance &mesh_instance);
		void Preprocess();
	};
}