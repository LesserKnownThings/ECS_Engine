#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace LKT
{
	namespace fs = std::filesystem;

	namespace FileHelper
	{
		static void GetFilesFromDirectory(const std::string &folderPath, std::vector<std::string> &files)
		{
			try
			{
				for (const auto &entry : fs::directory_iterator(folderPath))
				{
					if (entry.is_regular_file())
					{
						files.push_back(entry.path().string());
					}
				}
			}
			catch (const fs::filesystem_error &e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
			}
		}

		static void GetFilesFromDirectory(const std::string &folderPath, std::vector<fs::path> &files, bool recursive = true)
		{
			try
			{
				if (recursive)
				{
					for (const auto &entry : fs::recursive_directory_iterator(folderPath))
					{
						if (entry.is_regular_file())
						{
							files.push_back(entry.path());
						}
					}
				}
				else
				{
					for (const auto &entry : fs::directory_iterator(folderPath))
					{
						if (entry.is_regular_file())
						{
							files.push_back(entry.path());
						}
					}
				}
			}
			catch (const fs::filesystem_error &e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
			}
		}

		static void GetDirectoriesFromDirectory(const std::string &directoryPath, std::vector<std::string> &directories)
		{
			try
			{
				for (const auto &entry : fs::directory_iterator(directoryPath))
				{
					if (entry.is_directory())
					{
						directories.push_back(entry.path().string());
					}
				}
			}
			catch (const fs::filesystem_error &e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
			}
		}

		static std::vector<std::string> SplitString(const std::string &inString, char delimiter)
		{
			std::vector<std::string> tokens;
			std::istringstream iss(inString);
			std::string token;
			while (std::getline(iss, token, delimiter))
			{
				tokens.push_back(token);
			}
			return tokens;
		}
	}
}