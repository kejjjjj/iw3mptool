#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <list>


namespace __fs
{
	[[nodiscard]] std::string exe_file_name();
	[[nodiscard]] std::string exe_path();
	[[nodiscard]] std::string get_extension(const std::string& path);
	[[nodiscard]] std::string previous_directory(std::string& directory);
	[[nodiscard]] std::string get_file_name(const std::string& fullpath);
	[[nodiscard]] std::string get_file_name_no_extension(const std::string& fullpath);
	[[nodiscard]] bool create_directory(const std::string& path);
	[[nodiscard]] std::vector<std::string> files_in_directory(const std::string& path);
	[[nodiscard]] std::vector<std::string> items_in_directory_formatted(const std::string& path);
	[[nodiscard]] std::string get_last_error();
	[[nodiscard]] inline bool directory_exists(const std::string& d) { return std::filesystem::exists(d); }
	[[nodiscard]] inline bool file_exists(const std::string& f) { return std::filesystem::exists(f); }
	[[nodiscard]] bool valid_file_name(const std::string& name);

	void create_file(const std::string& path);

}
