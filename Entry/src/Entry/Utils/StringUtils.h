#pragma once
#include <string>

namespace Entry
{
	static std::string ExtractFileName(const std::string& path)
	{
		// Find last slash or backslash
		size_t slashPos = path.find_last_of("/\\");
		std::string filename = (slashPos == std::string::npos) ? path : path.substr(slashPos + 1);

		// Find last dot
		size_t dotPos = filename.find_last_of('.');
		if (dotPos == std::string::npos)
		{
			return filename; // no extension
		}
		return filename.substr(0, dotPos);
	}

	static std::string ExtractFileNameWithExtension(const std::string& path)
	{
		// Find last slash or backslash
		size_t slashPos = path.find_last_of("/\\");
		std::string filename = (slashPos == std::string::npos) ? path : path.substr(slashPos + 1);

		return filename;
	}

	static std::string ExtractDirectory(const std::string& path)
	{
		// Find last slash or backslash
		size_t slashPos = path.find_last_of("/\\");
		std::string directory = (slashPos == std::string::npos) ? path : path.substr(0, slashPos);

		return directory;
	}
}