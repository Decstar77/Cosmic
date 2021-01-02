
#include "Platform.h"
#include <fstream>
#include <filesystem>

namespace cm
{
	void Platform::ErrorBox(const String &msg)
	{
		ASSERT(instance != nullptr, "Platform doesn't exist");
		instance->ErrorBox_(msg);
	}

	void Platform::WarningBox(const String &msg)
	{
		ASSERT(instance != nullptr, "Platform doesn't exist");
		instance->WarningBox_(msg);
	}

	bool Platform::QuestionBox(const String &msg)
	{
		ASSERT(instance != nullptr, "Platform doesn't exist");
		return instance->QuestionBox_(msg);
	}

	int32 Platform::GetClientWidth()
	{
		ASSERT(instance != nullptr, "Platform doesn't exist");
		return instance->GetClientWidth_();
	}

	int32 Platform::GetClientHeight()
	{
		ASSERT(instance != nullptr, "Platform doesn't exist");
		return instance->GetClientHeight_();
	}

	bool Platform::IsWindowFocused()
	{
		ASSERT(instance != nullptr, "Platform doesn't exist");
		return instance->IsWindowFocused_();
	}

	FileResult Platform::LoadFile(const String &path)
	{
		// @NOTE: Returning by value is good here cause std::vector has move semanitcs

		std::ifstream file(path, std::ios::binary | std::ios::ate);

		ASSERT(file.is_open(), "Couldn't find open: " + path);

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		FileResult result;
		result.path = path;
		result.data.resize(size);

		if (!file.read(result.data.data(), size))
		{
			ASSERT(0, "Couldn't read data from: " + path);
		}

		return result;
	}

	FolderResult Platform::LoadFolder(const String &path, const String &file_type)
	{
		std::vector<String> paths;

		for (const auto &entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_regular_file() && entry.path().extension() == file_type)
			{
				paths.push_back((entry.path().generic_string()));
			}
		}

		FolderResult folder;
		folder.path = path;

		for (const String &file_path : paths)
		{
			FileResult file = Platform::LoadFile(file_path);
			folder.files.emplace_back(file);
		}

		LOG(path << " file count: " << paths.size());

		return folder;
	}

	void Platform::WriteTextFile(const String &path, const String &data)
	{
		std::ofstream myfile(path, std::ofstream::trunc);
		myfile << data;
		myfile.close();
	}

	StringStream Platform::ReadTextFile(const String &path)
	{
		StringStream ss;

		std::ifstream file(path);

		ss << file.rdbuf();

		file.close();

		return ss;
	}

	Platform::Platform()
	{
		ASSERT(instance == nullptr, "Can't have more than one platform");
		instance = this;
	}

	Platform::~Platform()
	{
		instance = nullptr;
	}

}
