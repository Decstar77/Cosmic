#pragma once
#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"
#include "src/rendering/Renderer.h"
#include "src/serialization/FileParsers.h"
#include "src/core/JobSystem.h"

namespace cm
{
#define THREADED 1

	template<typename T>
	struct ThreadedResult
	{
		bool valid;
		T data;
	};


	class AssetTable
	{
	public:

		//AtomicBool modeles_locked;
		RetainLock models_lock;
		std::queue<MeshEntry> mesh_load_requests;
		std::vector<MeshMetaData> mesh_meta_data;				// @TODO: Pre reserve this!!
		std::vector<MeshInstance> mesh_intances;				// @TODO: Pre reserve this!!
		std::vector<EditableMesh> mesh_raw_data;				// @TODO: Pre reserve this!!


		//************************************
		//
		//************************************
		ThreadedResult<EditableMesh> FindMeshData(const String &name);



		//std::array<EditableTexture, 2> textures;

		AssetTable() {};
		~AssetTable() {};

	public:
		MeshInstance GetMeshInstance(const MeshEntry &entry);

		//************************************
		// @NOTE: Asssumes the mesh is valid and is there
		// @NOTE: This method blocks if locked
		//************************************
		EditableMesh GetRawMesh(const MeshEntry &entry);


		//************************************
		// @NOTE: Is garuateed to be there with no blocking becaue we load this on main ahead of time
		// @Speed: O(n) as we have to loop through everything. Could make this faster with hash map
		//************************************
		MeshEntry FindMeshEntry(const String &name);


		//************************************
		// @NOTE:Is garuateed to be there with no blocking becaue we load this on main ahead of time
		// @SPEED: O(1), it's just an index loop up
		//************************************
		MeshMetaData GetMeshMetaData(const MeshEntry &entry);
	};


	class ModelLoadJob : public JobWork
	{
	public:
		virtual void Process() override;

	public:
		ModelLoadJob(const FileResult &file_result, const MeshEntry &mesh_entry, AssetTable *asset_table);
		~ModelLoadJob();

	private:
		AssetTable *asset_table;
		FileResult mesh_file;
		MeshEntry mesh_entry;

	};

	class AssetLoader
	{
	public:
		void LoadTexture(const String &path);
		void LoadModel(const String &path);
		void LoadEntireFolderOfModels(const String &path);
		void LoadEnitreFolderOfTextures(const String &path);

	public:
		AssetLoader(AssetTable *asset_table, const String &resource_path);
		~AssetLoader();

	private:
		String resource_path;
		AssetTable *asset_table;

		std::thread worker;
		std::list<ModelLoadJob> model_jobs;
		//std::queue<String> load_queue;

		inline static AssetLoader *instance = nullptr;

	private:
		void DoLoadEntireFolderOfModels(FolderResult mesh_folder);
		void ParseModel(const FileResult &mesh_file, const MeshEntry &mesh_entry);
		void Preprocess();
	};
}