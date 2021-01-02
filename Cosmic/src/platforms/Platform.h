#pragma once
#include "src/Cosmic.h"

namespace cm
{
	struct FileResult
	{
		String path;
		std::vector<char> data;
	};

	struct FolderResult
	{
		String path;
		std::vector<FileResult> files;
	};

	class Platform
	{
	public:
		static void ErrorBox(const String &msg);
		static void WarningBox(const String &msg);
		static bool QuestionBox(const String &msg);

		static int32 GetClientWidth();
		static int32 GetClientHeight();

		static bool IsWindowFocused();

		static FileResult LoadFile(const String &path);
		static FolderResult LoadFolder(const String &path, const String &file_type);

		static void WriteTextFile(const String &path, const String &data);
		static StringStream ReadTextFile(const String &path);

	protected:
		virtual void ErrorBox_(const String &msg) = 0;
		virtual void WarningBox_(const String &msg) = 0;
		virtual bool QuestionBox_(const String &msg) = 0;

		virtual int32 GetClientWidth_() = 0;
		virtual int32 GetClientHeight_() = 0;

		virtual bool IsWindowFocused_() = 0;

	public:
		Platform();
		Platform(const Platform &) = delete;
		Platform &operator=(const Platform &) = delete;
		virtual ~Platform();

	private:
		inline static Platform *instance = nullptr;

	};
}
