#include "Assets.h"

namespace cm
{
	void AssetLoader::Preprocess()
	{
		String path = resource_path + "/meshes";
		FolderResult meta_mesh = Platform::LoadFolder(path, ".meta");

		MeshEntry current_entry = 0;

		for (FileResult &meta_file : meta_mesh.files)
		{
			StringStream ss;
			ss << (char *)meta_file.data.data();

			String line;
			MeshMetaData meta_data;
			meta_data.entry = current_entry;
			meta_data.base_scale = 1.0f;
			meta_data.path = meta_file.path;

			int32 name_pos = (int32)meta_file.path.find_last_of('/');
			if (name_pos != std::string::npos)
			{
				meta_data.name = meta_file.path.substr(name_pos + 1);
				int32 ext_pos = (int32)meta_data.name.find_first_of(".");
				if (ext_pos != std::string::npos)
				{
					meta_data.name = meta_data.name.substr(0, ext_pos);
				}
			}

			while (std::getline(ss, line))
			{
				if (line.find("Min: ") != std::string::npos)
				{
					line = line.erase(0, 5);
					meta_data.bounding_box.min = StringToVec3<real32>(line);
				}
				else if (line.find("Max: ") != std::string::npos)
				{
					line = line.erase(0, 5);
					meta_data.bounding_box.max = StringToVec3<real32>(line);
				}
				else if (line.find("Position count: ") != std::string::npos)
				{
					line = line.erase(0, 16);
					meta_data.position_count = std::stoi(line);
				}
				else if (line.find("Normal count: ") != std::string::npos)
				{
					line = line.erase(0, 14);
					meta_data.normal_count = std::stoi(line);
				}
				else if (line.find("TexCoords count: ") != std::string::npos)
				{
					line = line.erase(0, 17);
					meta_data.tcoords_count = std::stoi(line);
				}
				else if (line.find("Faces count: ") != std::string::npos)
				{
					line = line.erase(0, 13);
					meta_data.face_count = std::stoi(line);
				}
			}

			asset_table->mesh_meta_data.emplace_back(meta_data);

			current_entry++;
		}

		asset_table->mesh_raw_data.resize(meta_mesh.files.size());
		asset_table->mesh_intances.resize(meta_mesh.files.size());
	}

	void AssetLoader::LoadModel(const String &path)
	{
		ASSERT(0, "AssetLoader::LoadModel incomplete");
		//FileResult mesh_file = Platform::LoadFile(path);
		//ParseModel(mesh_file, );
	}

	void AssetLoader::LoadEntireFolderOfModels(const String &path)
	{
		FolderResult mesh_folder = Platform::LoadFolder(path, ".obj");
#if THREADED
		worker = std::thread(&AssetLoader::DoLoadEntireFolderOfModels, this, mesh_folder);
#else
		AssetLoader::DoLoadEntireFolderOfModels(mesh_folder);
#endif
	}

	AssetLoader::AssetLoader(AssetTable *asset_table, const String &resource_path)
		: asset_table(asset_table), resource_path(resource_path)
	{
		Preprocess();
	}

	AssetLoader::~AssetLoader()
	{
		if (worker.joinable()) { worker.join(); }
	}

	void AssetLoader::DoLoadEntireFolderOfModels(FolderResult mesh_folder)
	{
		MeshEntry entry = 0;
		for (FileResult &mesh_file : mesh_folder.files)
		{
			ParseModel(mesh_file, entry);
			entry++;
		}
	}

	void AssetLoader::ParseModel(const FileResult &mesh_file, const MeshEntry &mesh_entry)
	{
		MeshMetaData mesh_data = asset_table->GetMeshMetaData(mesh_entry);

		EditableMesh mesh = EditableMesh(mesh_file, mesh_data, MeshFileType::OBJ);

		asset_table->models_lock.Lock();

		asset_table->mesh_load_requests.emplace(mesh_entry);
		asset_table->mesh_raw_data.at(mesh_entry) = mesh;

		asset_table->models_lock.Unlock();
	}

	ThreadedResult<EditableMesh> AssetTable::FindMeshData(const String &name)
	{
		ThreadedResult<EditableMesh> mesh;
		mesh.valid = false;

		for (MeshMetaData &data : mesh_meta_data)
		{
			if (data.name == name)
			{
				models_lock.Lock();
				if (this->mesh_raw_data.at(data.entry).IsValid())
				{
					mesh.data = this->mesh_raw_data.at(data.entry);
					mesh.valid = true;
				}
				models_lock.Unlock();
				break;
			}
		}

		return mesh;
	}

	MeshEntry AssetTable::FindMeshEntry(const String &name)
	{
		for (MeshMetaData &data : mesh_meta_data)
		{
			if (data.name == name)
			{
				return data.entry;
			}
		}

		return {};
	}

	EditableMesh AssetTable::GetRawMesh(const MeshEntry &entry)
	{
		ASSERT(entry >= 0, "AssetTable::GetMesh tried to get invalid entry");

		models_lock.Lock();

		EditableMesh mesh = mesh_raw_data.at(entry);

		models_lock.Unlock();

		return mesh;
	}

	MeshMetaData AssetTable::GetMeshMetaData(const MeshEntry &entry)
	{
		ASSERT(entry >= 0, "AssetTable::GetMesh tried to get invalid entry");

		MeshMetaData mesh = mesh_meta_data.at(entry);

		return mesh;
	}

}