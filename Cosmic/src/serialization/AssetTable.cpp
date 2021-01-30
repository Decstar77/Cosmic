#include "AssetTable.h"

namespace cm
{
	void AssetLoader::Preprocess()
	{
		String path = resource_path + "/meshes";
		FolderResult meta_mesh = Platform::LoadFolder(path, ".meta");

		int32 current_entry_counter = 0;

		for (FileResult &meta_file : meta_mesh.files)
		{
			StringStream ss;
			ss << (char *)meta_file.data.data();

			String line;
			MeshMetaData meta_data;
			meta_data.base_scale = 1.0f;

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

			MeshInstance mesh_instance;
			mesh_instance.meta_data = meta_data;
			mesh_instance.asset_table_index = current_entry_counter;

			asset_table->mesh_intances.push_back(mesh_instance);

			current_entry_counter++;
		}

		asset_table->editable_meshes.resize(meta_mesh.files.size());
	}

	void AssetLoader::LoadModel(const String &path)
	{
		ASSERT(0, "AssetLoader::LoadModel incomplete");
		//FileResult mesh_file = Platform::LoadFile(path);
		//ParseModel(mesh_file, );
	}

	void AssetLoader::LoadShader(const String &vertex_path, const String &pixel_path)
	{
		asset_table->shader_instances.emplace_back();
		asset_table->editable_shaders.emplace_back(
			Platform::LoadFile(vertex_path),
			Platform::LoadFile(pixel_path)
		);
	}

	void AssetLoader::LoadTexture(const String &path)
	{
		TextureInstance &inst = asset_table->texture_instances.emplace_back();
		EditableTexture &text = asset_table->editable_textures.emplace_back(
			Platform::LoadFile(path)
		);

		inst.meta_data = text.meta_data;
	}

	void AssetLoader::LoadEntireFolderOfModels(const String &path)
	{
		FolderResult mesh_folder = Platform::LoadFolder(path, ".obj");
#if THREADED
		for (int32 i = 0; i < asset_table->mesh_intances.size(); i++)
		{
			MeshInstance instance = asset_table->mesh_intances.at(i);
			FileResult &mesh_file = mesh_folder.files.at(i);

			ModelLoadJob *job = &model_jobs.emplace_back(mesh_file, instance, asset_table);

			JobSystem::GetInstance()->AddJob(job);
		}
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
	}

	void AssetLoader::DoLoadEntireFolderOfModels(FolderResult mesh_folder)
	{
		for (int32 i = 0; i < asset_table->mesh_intances.size(); i++)
		{
			MeshInstance instance = asset_table->mesh_intances.at(i);
			FileResult &mesh_file = mesh_folder.files.at(i);

			ParseModel(mesh_file, instance);
		}
	}

	void AssetLoader::ParseModel(const FileResult &mesh_file, const MeshInstance &mesh_instance)
	{
		MeshMetaData mesh_data = mesh_instance.meta_data;

		EditableMesh mesh = EditableMesh(mesh_file, mesh_data, MeshFileType::OBJ);

		asset_table->editable_meshes.at(mesh_instance.asset_table_index) = mesh;
	}

	MeshInstance AssetTable::FindMeshInstance(const String &name)
	{
		for (MeshInstance &instance : mesh_intances)
		{
			if (instance.meta_data.name == name)
			{
				return instance;
			}
		}

		return {};
	}

	EditableMesh AssetTable::GetEditableMesh(const MeshInstance &instance)
	{
		ASSERT(instance.asset_table_index >= 0, "AssetTable::GetMesh tried to get invalid entry");

		EditableMesh mesh = editable_meshes.at(instance.asset_table_index);

		return mesh;
	}

	ModelLoadJob::ModelLoadJob(const FileResult &file_result, const MeshInstance &mesh_instance, AssetTable *asset_table)
		: JobWork("ModelLoader"), mesh_file(file_result), mesh_instance(mesh_instance), asset_table(asset_table)
	{

	}

	ModelLoadJob::~ModelLoadJob()
	{

	}

	void ModelLoadJob::Process()
	{
		EditableMesh mesh = EditableMesh(mesh_file, mesh_instance.meta_data, MeshFileType::OBJ);

		asset_table->editable_meshes.at(mesh_instance.asset_table_index) = mesh;
	}
	}